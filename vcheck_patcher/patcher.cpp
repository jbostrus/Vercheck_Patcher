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

Patterns PatternNewpatchLocation =
{	// 48 8B F9 0F 2E 35 ? ? ? ? 
	"\x48\x8B\xF9\x0F\x2E\x35\x00\x00\x00\x00\x0F\x84\x00\x00\x00\x00\x48\x83\xB9\x00\x00\x00\x00\x00\x0F\x85\x00\x00\x00\x00\x83\x3D\x00\x00\x00\x00\x00",
	"xxxxxx????xx????xxx?????xx????xx?????",
	6
};

bool PatchMemory()
{
	MODULEINFO moduleInfo;
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &moduleInfo, sizeof(moduleInfo));
	
	uintptr_t addrToPatch = FindPattern((uintptr_t)moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage, (uint8_t*)PatternCheckerLocation.BytesToFind, PatternCheckerLocation.FindMask);
	if (!addrToPatch)
	{
		MessageBox(NULL, "Unable to find new original check pointer.", "Fatal Error", MB_OK | MB_ICONERROR);
		// TODO: fatal error couldn't find patch location.
		return false;
	}
	addrToPatch += PatternCheckerLocation.AddressModifier;

	uintptr_t addrNewValuePointer = FindPattern((uintptr_t)moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage, (uint8_t*)PatternNewpatchLocation.BytesToFind, PatternNewpatchLocation.FindMask);
	if (!addrNewValuePointer)
	{
		MessageBox(NULL, "Unable to find new value pointer.", "Fatal Error", MB_OK | MB_ICONERROR);
		// TODO: fatal error
		return false;
	}
	
	addrNewValuePointer += PatternNewpatchLocation.AddressModifier;	// actual address of the memory offset we want
	
	uint32_t addrOffsetVal = *(uint32_t*)addrNewValuePointer;	// temp set to relative offset of pattern match
	uintptr_t addrAbsOffset = addrNewValuePointer + addrOffsetVal + sizeof(addrOffsetVal);

	uintptr_t addrPatchRelOffset = addrAbsOffset - addrToPatch - sizeof(addrOffsetVal);

#ifdef _DEBUG
	/**
	
	This code is intended to help with debugging to validate the logic above is
	finding the correct locations from the patterns.

	**/
	sizeof(float);
	float fVal = *(float*)addrAbsOffset;
	fVal = fVal * 1000;
	char buf[2048];
	uintptr_t addrRelOffset = addrAbsOffset - (uintptr_t)moduleInfo.lpBaseOfDll;
	sprintf_s(buf, "Found game base address at: %X\n" 
		"Relative offset to patch: %X\n" 
		"Relative offset to get new value from: %X\n" 
		"Offset of real new value from new pointer: %X\n" 
		"New value at: %X\n"
		"Value of: %d\n"
		"Patch offset val: %X",
		(unsigned int)moduleInfo.lpBaseOfDll,
		(unsigned int)(addrToPatch - (uintptr_t)moduleInfo.lpBaseOfDll),
		(unsigned int)(addrNewValuePointer - (uintptr_t)moduleInfo.lpBaseOfDll),
		(unsigned int)addrOffsetVal,
		(unsigned int)addrRelOffset,
		(unsigned int)fVal,
		(unsigned int)addrPatchRelOffset
	);
	MessageBox(NULL, buf, "TEST", MB_OK | MB_ICONEXCLAMATION);
#endif

	PatchMemory(addrToPatch, reinterpret_cast<uint8_t*>(&addrPatchRelOffset), 4);

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
