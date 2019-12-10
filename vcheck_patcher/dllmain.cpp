#include "common/IPrefix.h"
#include "f4se/PluginAPI.h"
#include "f4se_common/f4se_version.h"

#define VERSION_TO_STRING(a) std::to_string(GET_EXE_VERSION_MAJOR(a)) + "." + std::to_string(GET_EXE_VERSION_MINOR(a)) + "." + std::to_string(GET_EXE_VERSION_BUILD(a)) + (GET_EXE_VERSION_SUB(a) > 0? "." + GET_EXE_VERSION_SUB(a): "");

const char pluginName[] = { "VcheckPatcher" };
const UInt32 pluginVersion = MAKE_EXE_VERSION_EX(1, 0, 0, 0);
const std::string pluginVersionString = VERSION_TO_STRING(pluginVersion);

bool PatchMemory();

PluginHandle g_pluginHandle = kPluginHandle_Invalid;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C" {
	bool F4SEPlugin_Query(const F4SEInterface* f4se, PluginInfo* info) {
		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = pluginName;
		info->version = pluginVersion;

		if (f4se->runtimeVersion != RUNTIME_VERSION_1_10_138) {
			UInt32 runtimeVersion = RUNTIME_VERSION_1_10_138;
			char buf[512];
			sprintf_s(buf, "vcheck patcher\nExpected Version: %d.%d.%d.%d\nFound Version: %d.%d.%d.%d",
				GET_EXE_VERSION_MAJOR(runtimeVersion),
				GET_EXE_VERSION_MINOR(runtimeVersion),
				GET_EXE_VERSION_BUILD(runtimeVersion),
				GET_EXE_VERSION_SUB(runtimeVersion),
				GET_EXE_VERSION_MAJOR(f4se->runtimeVersion),
				GET_EXE_VERSION_MINOR(f4se->runtimeVersion),
				GET_EXE_VERSION_BUILD(f4se->runtimeVersion),
				GET_EXE_VERSION_SUB(f4se->runtimeVersion));
			MessageBox(NULL, buf, "Game Version Error", MB_OK | MB_ICONEXCLAMATION);
			_FATALERROR("unsupported runtime version %08X", f4se->runtimeVersion);
			return false;
		}

		// store plugin handle so we can identify ourselves later
		g_pluginHandle = f4se->GetPluginHandle();
		if (f4se->isEditor) {
			_MESSAGE("loaded in editor, marking as incompatible");
			return false;
		}

		// supported runtime version
		_MESSAGE("%s query successful.", pluginName);
		return true;
	}

	bool F4SEPlugin_Load(const F4SEInterface* f4se) {
		_MESSAGE("%s loading...", pluginName);
		PatchMemory();
		_MESSAGE("%s load successful.", pluginName);
		return true;
	}
};