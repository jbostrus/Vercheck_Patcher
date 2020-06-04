#if defined(F4SE) || defined(SKSE64)
#include "common/IPrefix.h"
#endif /* F4SE || SKSE64 */
#ifdef _DXGI_SHIM
#include <Windows.h>
#endif
#ifdef _DEBUG
#include <Psapi.h>
#pragma comment( lib, "psapi.lib" )
#endif /* DEBUG */
#include "config.h"
#include "logging.h"
#include "patcher.h"
#include "rva/RVA.h"
#include "Utils.h"

bool TryToPatchMemory()
{
#if _SKYRIM64
	// v1.5.97 @ 0x17B890 base search + 11 for target
	RVA<uintptr_t> PatternAddressToPatch("48 81 EC ? ? ? ? F3 0F 10 05 ? ? ? ? 0F 2F 81 ? ? ? ?", 11);
	// v1.5.97 @ 0x5BD6EB base search + 4 for constant offset location
	RVA<uintptr_t> PatternNewValueOffsetLocation("F3 0F 59 0D ? ? ? ? 48 8B 4F 08", 4);
#elif _FALLOUT4
	// v1.10.138 @ 0x17B890 base search + 4 for target
	RVA<uintptr_t> PatternAddressToPatch("F3 0F 10 35 ? ? ? ? 0F 2F B6 ? ? ? ?", 4);
	// v1.10.138 @ 0x132E93 base search + 12 for constant offset location
	RVA<uintptr_t> PatternNewValueOffsetLocation("F3 0F 10 0D ? ? ? ? F3 0F 59 0D ? ? ? ? F3 0F 59 0D ? ? ? ?", 12);
#endif /* FALLOUT4 || SKYRIM64 */

	// resolve all address searches
	RVAManager::UpdateAddresses(0);

	if (!PatternAddressToPatch.IsResolved())
	{
		MessageBox(NULL, "Unable to find original check location that needs patch.", "Fatal Error", MB_OK | MB_ICONERROR);
		return false;
	}
	// Address pointer to patch new constant location to
	uintptr_t ptrAbsAddressToPatch = PatternAddressToPatch.GetUIntPtr();

	if (!PatternNewValueOffsetLocation.IsResolved())
	{
		MessageBox(NULL, "Unable to find new value to patch in.", "Fatal Error", MB_OK | MB_ICONERROR);
		return false;
	}
	// Address pointer where we get the child location for our new contant
	uintptr_t ptrAbsAddressNewValuePointerSearch = PatternNewValueOffsetLocation.GetUIntPtr();

	// Absolute pointer address to our desired constant
	uintptr_t ptrAbsAddressNewConstant = Utils::GetRelative(ptrAbsAddressNewValuePointerSearch, 0, 4);
	
	// calculate the relative offset from patch address to new constant
	// Address offset of new constant relative to patch address
	int32_t offNewConstRelToPatchAddress = (int32_t)(ptrAbsAddressNewConstant - ptrAbsAddressToPatch - sizeof(int32_t));

#ifdef _DEBUG
	/**
		This code is intended to help with debugging to validate the logic
		above is finding the correct locations from the patterns.
	**/
#define DEBUGTITLE "DEBUG " PLUGIN_NAME_LONG " " PLUGIN_VERSION_VERSTRING

	MODULEINFO moduleInfo;
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &moduleInfo, sizeof(moduleInfo));

	float fVal;
	Utils::ReadMemory(ptrAbsAddressNewConstant, &fVal, sizeof(fVal));
	char buf[2048];
	snprintf(buf, sizeof(buf),
		"Found game base address at: 0x%I64X\n"
		"Relative offset to patch location: 0x%08I64X\n"
		"Relative offset to grab new constant pointer from: 0x%08I64X\n"
		"Relative offset to new value const: 0x%08I64X\n"
		"Float value of new constant: %d.%04d\n"
		"Offset value to patch in: 0x%08I64X",
		(intptr_t)moduleInfo.lpBaseOfDll,
		(intptr_t)(ptrAbsAddressToPatch - (intptr_t)moduleInfo.lpBaseOfDll),
		(intptr_t)(ptrAbsAddressNewValuePointerSearch - (intptr_t)moduleInfo.lpBaseOfDll),
		(intptr_t)ptrAbsAddressNewConstant - (intptr_t)moduleInfo.lpBaseOfDll,
		(unsigned int)fVal, (unsigned int)((fVal - (unsigned int)fVal) * 1000),
		(intptr_t)offNewConstRelToPatchAddress
	);
	_MESSAGE(buf);
	MessageBox(NULL, buf, DEBUGTITLE, MB_OK | MB_ICONEXCLAMATION);
#endif

	Utils::WriteMemory(ptrAbsAddressToPatch, &offNewConstRelToPatchAddress, sizeof(offNewConstRelToPatchAddress));
	return true;
}
