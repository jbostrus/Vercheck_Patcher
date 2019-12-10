#include "common/IPrefix.h"
#include "f4se/PluginAPI.h"
#include "f4se_common/f4se_version.h"
#include "patcher.h"

#define VERSION_TO_STRING(a) std::to_string(GET_EXE_VERSION_MAJOR(a)) + "." + std::to_string(GET_EXE_VERSION_MINOR(a)) + "." + std::to_string(GET_EXE_VERSION_BUILD(a)) + (GET_EXE_VERSION_SUB(a) > 0? "." + GET_EXE_VERSION_SUB(a): "");

const char pluginName[] = { "VcheckPatcher" };
const UInt32 pluginVersion = MAKE_EXE_VERSION_EX(1, 0, 0, 0);
const std::string pluginVersionString = VERSION_TO_STRING(pluginVersion);

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	return TRUE;
}

extern "C" {
	bool F4SEPlugin_Query(const F4SEInterface* f4se, PluginInfo* info)
	{
		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = pluginName;
		info->version = pluginVersion;

		if (f4se->isEditor) {
			_MESSAGE("loaded in editor, marking as incompatible");
			return false;
		}

		// supported runtime version
		_MESSAGE("%s query successful.", pluginName);
		return true;
	}

	bool F4SEPlugin_Load(const F4SEInterface* f4se)
	{
		_MESSAGE("%s loading...", pluginName);
		if (!TryToPatchMemory())
		{
			MessageBox(NULL, "Something went terribly wrong patching memory. The game will probably crash or behave incorrectly.", pluginName, MB_OK | MB_ICONERROR);
			_MESSAGE("%s load failed epicly.", pluginName);
			return false;
		}
		_MESSAGE("%s load successful.", pluginName);
		return true;
	}
};