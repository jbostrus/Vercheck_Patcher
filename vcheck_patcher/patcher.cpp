#if defined(F4SE) || defined(SKSE64)
#include "common/IPrefix.h"
#endif /* F4SE || SKSE64 */
#include <Windows.h>
#include <vector>
#include <algorithm>
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
	// Address pointer to patch new constant location to
	uintptr_t ptrAbsAddressToPatch;
	// Address pointer where we get the child location for our new contant
	uintptr_t ptrAbsAddressNewValuePointerSearch;
	// Address offset of new constant relative to patch address
	uint32_t offNewConstRelToPatchAddress;
	// Absolute pointer address to our desired constant
	uintptr_t ptrAbsAddressNewConstant;

#ifdef _SKYRIM64
	// v1.5.97 @ 0x17B890 base search + 11 for target
	RVA<uintptr_t> PatternCheckerLocation("48 81 EC ? ? ? ? F3 0F 10 05 ? ? ? ? 0F 2F 81 ? ? ? ?", 11);
	// v1.5.97 @ 0x5BD6EB base search + 4 for constant offset location
	RVA<uintptr_t> PatternNewValueSearchLocation("F3 0F 59 0D ? ? ? ? 48 8B 4F 08", 4);
#elif _FALLOUT4
	// v1.10.138 @ 0x17B890 base search + 4 for target
	RVA<uintptr_t> PatternCheckerLocation("F3 0F 10 35 ? ? ? ? 0F 2F B6 ? ? ? ?", 4);
	// v1.10.138 @ 0x132E93 base search + 12 for constant offset location
	RVA<uintptr_t> PatternNewValueSearchLocation("F3 0F 10 0D ? ? ? ? F3 0F 59 0D ? ? ? ? F3 0F 59 0D ? ? ? ?", 12);
#endif /* FALLOUT4 || SKYRIM64 */

	// resolve all address searches
	RVAManager::UpdateAddresses(0);

	if (!PatternCheckerLocation.IsResolved())
	{
		MessageBox(NULL, "Unable to find new original check location that needs patch.", "Fatal Error", MB_OK | MB_ICONERROR);
		return false;
	}
	ptrAbsAddressToPatch = PatternCheckerLocation.GetUIntPtr();

	if (!PatternNewValueSearchLocation.IsResolved())
	{
		MessageBox(NULL, "Unable to find new value to patch in.", "Fatal Error", MB_OK | MB_ICONERROR);
		return false;
	}
	ptrAbsAddressNewValuePointerSearch = PatternNewValueSearchLocation.GetUIntPtr();

	// calculate the relative offset from patch address to new constant
	ptrAbsAddressNewConstant = Utils::GetRelative(ptrAbsAddressNewValuePointerSearch, 0, 4);
	offNewConstRelToPatchAddress = (uint32_t)(ptrAbsAddressNewConstant - ptrAbsAddressToPatch - sizeof(uint32_t));

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
		(int64_t)moduleInfo.lpBaseOfDll,
		(int64_t)(ptrAbsAddressToPatch - (uintptr_t)moduleInfo.lpBaseOfDll),
		(int64_t)(ptrAbsAddressNewValuePointerSearch - (uintptr_t)moduleInfo.lpBaseOfDll),
		(int64_t)ptrAbsAddressNewConstant - (uintptr_t)moduleInfo.lpBaseOfDll,
		(unsigned int)fVal, (unsigned int)((fVal - fVal) * 1000),
		(int64_t)offNewConstRelToPatchAddress
	);
	_MESSAGE(buf);
	MessageBox(NULL, buf, DEBUGTITLE, MB_OK | MB_ICONEXCLAMATION);
#endif

	Utils::WriteMemory(ptrAbsAddressToPatch, &offNewConstRelToPatchAddress, sizeof(offNewConstRelToPatchAddress));
	return true;
}
