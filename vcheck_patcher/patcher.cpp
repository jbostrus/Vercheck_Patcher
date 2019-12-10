#include "common/IPrefix.h"
#include <vector>
#include <algorithm>

bool PatchMemory();
void PatchMemory(uintptr_t Address, uint8_t* Data, size_t Size);

bool PatchMemory() {

	uintptr_t g_ModuleBase = (uintptr_t)GetModuleHandle(nullptr);
	uint32_t offsetNewVersion = 0x2C74554 - 0x1303AE; // ptr to static value 1.0f

	PatchMemory(g_ModuleBase + 0x1303A6 + 4, reinterpret_cast<uint8_t*>(&offsetNewVersion), 4);

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
