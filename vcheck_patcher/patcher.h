#pragma once

struct Patterns
{
	const char* BytesToFind;
	const char* FindMask;
	int AddressModifier;
};

bool PatchMemory();
void PatchMemory(uintptr_t Address, uint8_t* Data, size_t Size);
uintptr_t FindPattern(uintptr_t StartAddress, uintptr_t MaxSize, const uint8_t* Bytes, const char* Mask);
