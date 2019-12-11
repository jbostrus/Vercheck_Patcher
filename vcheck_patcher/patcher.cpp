#include "common/IPrefix.h"
#include "patcher.h"
#include <vector>
#include <algorithm>
#include <Psapi.h>
#pragma comment( lib, "psapi.lib" )

Patterns PatternCheckerLocation =
{	// F3 0F 10 35 ?? ?? ?? ?? 0F 2F B6 ?? ?? ?? ?? 0F 82 ?? ?? ?? ??
	"\xF3\x0F\x10\x35\x00\x00\x00\x00\x0F\x2F\xB6\x00\x00\x00\x00\x0F\x82\x00\x00\x00\x00",	// location comparison pattern
	"xxxx????xxx????xx????",
	4	// offset to patch at
};

// this pattern finds a known constant for a 1024.0 float
Patterns PatternNewValueSearchLocation =
{
	"\xF3\x0F\x10\x0D\x00\x00\x00\x00\xF3\x0F\x59\x0D\x00\x00\x00\x00\xF3\x0F\x59\x0D\x00\x00\x00\x00\xF3\x48\x0F\x2C\xC1\x89\x81\x00\x00\x00\x00\x48\x8B\x49\x50\x8B\xD0\xE8\x00\x00\x00\x00",
	"xxxx????xxxx????xxxx????xxxxxxx????xxxxxxx????",
	12	// offset from search to target const offset
};

bool TryToPatchMemory()
{
	MODULEINFO moduleInfo;
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &moduleInfo, sizeof(moduleInfo));
	
	// Address pointer to patch new constant location to
	uintptr_t ptrAbsAddressToPatch;
	// Address pointer where we get the child location for our new contant
	uintptr_t ptrAbsAddressNewValuePointerSearch;
	// Address offset of new constant relative to patch address
	uint32_t offNewConstRelToPatchAddress;
	// Absolute pointer address to our desired constant
	uintptr_t ptrAbsAddressNewConstant;

	ptrAbsAddressToPatch = FindPattern((uintptr_t)moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage, (uint8_t*)PatternCheckerLocation.BytesToFind, PatternCheckerLocation.FindMask);
	if (!ptrAbsAddressToPatch)
	{
		MessageBox(NULL, "Unable to find new original check pointer.", "Fatal Error", MB_OK | MB_ICONERROR);
		return false;
	}
	ptrAbsAddressToPatch += PatternCheckerLocation.AddressModifier;

	ptrAbsAddressNewValuePointerSearch = FindPattern((uintptr_t)moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage, (uint8_t*)PatternNewValueSearchLocation.BytesToFind, PatternNewValueSearchLocation.FindMask);
	if (!ptrAbsAddressNewValuePointerSearch)
	{
		MessageBox(NULL, "Unable to find new value pointer.", "Fatal Error", MB_OK | MB_ICONERROR);
		return false;
	}
	
	ptrAbsAddressNewValuePointerSearch += PatternNewValueSearchLocation.AddressModifier;
	
	// temp helper to calculate relative offset from pattern match address
	uint32_t offNewConstRelToNewSearchAddr = *(uint32_t*)ptrAbsAddressNewValuePointerSearch;
	ptrAbsAddressNewConstant = ptrAbsAddressNewValuePointerSearch + offNewConstRelToNewSearchAddr + sizeof(offNewConstRelToNewSearchAddr);

	offNewConstRelToPatchAddress = (uint32_t)(ptrAbsAddressNewConstant - ptrAbsAddressToPatch - sizeof(offNewConstRelToNewSearchAddr));

#ifdef _DEBUG
	/**
	
	This code is intended to help with debugging to validate the logic above is
	finding the correct locations from the patterns.

	**/
	float fVal = *(float*)ptrAbsAddressNewConstant;
	char buf[2048];
	sprintf_s(buf, "Found game base address at: %I64X\n" 
		"Relative offset to patch location: %I64X\n" 
		"Relative offset to grab new constant pointer from: %I64X\n" 
		"Offset from new pointer to new const: %I64X\n" 
		"Offset from base to new value const at: %I64X\n"
		"Float value of new const: %d.%04d\n"
		"Offset value to patch in: %I64X",
		(int64_t)moduleInfo.lpBaseOfDll,
		(int64_t)(ptrAbsAddressToPatch - (uintptr_t)moduleInfo.lpBaseOfDll),
		(int64_t)(ptrAbsAddressNewValuePointerSearch - (uintptr_t)moduleInfo.lpBaseOfDll),
		(int64_t)offNewConstRelToNewSearchAddr,
		(int64_t)ptrAbsAddressNewConstant - (uintptr_t)moduleInfo.lpBaseOfDll,
		(unsigned int)fVal, (unsigned int)((fVal - fVal)*1024),
		(int64_t)offNewConstRelToPatchAddress
	);
	MessageBox(NULL, buf, "DEBUG vcheck", MB_OK | MB_ICONEXCLAMATION);
#endif

	PatchMemory(ptrAbsAddressToPatch, reinterpret_cast<uint8_t*>(&offNewConstRelToPatchAddress), 4);

	return true;
}

// PatchMemory snippet from Nukem's Skyrim tool code (XUtil) https://github.com/Nukem9/SkyrimSETest
void PatchMemory(uintptr_t Address, uint8_t* Data, size_t Size)
{
	DWORD d = 0;
	VirtualProtect((LPVOID)Address, Size, PAGE_EXECUTE_READWRITE, &d);

	for (uintptr_t i = Address; i < (Address + Size); i++)
		*(volatile uint8_t*)i = *Data++;

	VirtualProtect((LPVOID)Address, Size, d, &d);
	FlushInstructionCache(GetCurrentProcess(), (LPVOID)Address, Size);
}

// FindPattern snippet from Nukem's Skyrim tool code (XUtil) https://github.com/Nukem9/SkyrimSETest
uintptr_t FindPattern(uintptr_t StartAddress, uintptr_t MaxSize, const uint8_t* Bytes, const char* Mask)
{
	std::vector<std::pair<uint8_t, bool>> pattern;

	for (size_t i = 0; i < strlen(Mask); i++)
		pattern.emplace_back(Bytes[i], Mask[i] == 'x');

	const uint8_t* dataStart = (uint8_t*)StartAddress;
	const uint8_t* dataEnd = (uint8_t*)StartAddress + MaxSize + 1;

	auto ret = std::search(dataStart, dataEnd, pattern.begin(), pattern.end(),
		[](uint8_t CurrentByte, std::pair<uint8_t, bool>& Pattern)
		{
			return !Pattern.second || (CurrentByte == Pattern.first);
		});

	if (ret == dataEnd)
		return 0;

	return std::distance(dataStart, ret) + StartAddress;
}
