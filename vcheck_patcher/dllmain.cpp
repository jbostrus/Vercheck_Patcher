#ifndef _DXGI_SHIM	// disable this code if we're building dxgi loader
#include "f4se/PluginAPI.h"
#include "f4se_common/f4se_version.h"
#include "config.h"
#include "patcher.h"
#include <string>
#include <shlobj.h>

#define VERSION_TO_STRING(a) std::to_string(GET_EXE_VERSION_MAJOR(a)) + "." + std::to_string(GET_EXE_VERSION_MINOR(a)) + "." + std::to_string(GET_EXE_VERSION_BUILD(a)) + (GET_EXE_VERSION_SUB(a) > 0? "." + GET_EXE_VERSION_SUB(a): "");

IDebugLog gLog;
PluginHandle g_pluginHandle = kPluginHandle_Invalid;
const UInt32 pluginVersion = MAKE_EXE_VERSION_EX(PLUGIN_VERSION_INTEGER, PLUGIN_VERSION_INTEGER_MINOR, PLUGIN_VERSION_INTEGER_BETA, 0);
const std::string pluginVersionString = VERSION_TO_STRING(pluginVersion);

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		break;

	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C" {
	__declspec(dllexport) bool F4SEPlugin_Query(const F4SEInterface* f4se, PluginInfo* info)
	{
		std::string logPath = "\\My Games\\Fallout4\\F4SE\\";
		logPath += PLUGIN_LOGFILE_NAME;

		gLog.OpenRelative(CSIDL_MYDOCUMENTS, logPath.c_str());

		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = PLUGIN_NAME_SHORT;
		info->version = pluginVersion;

		// store plugin handle so we can identify ourselves later
		g_pluginHandle = f4se->GetPluginHandle(); 
		
		if (f4se->isEditor) {
			_MESSAGE("loaded in editor, marking as incompatible");
			return false;
		}

		// supported runtime version
		_MESSAGE("%s query successful.", PLUGIN_NAME_LONG);
		return true;
	}

	__declspec(dllexport) bool F4SEPlugin_Load(const F4SEInterface* f4se)
	{
		_MESSAGE("%s loading...", PLUGIN_NAME_LONG);
		if (!TryToPatchMemory())
		{
			MessageBox(NULL, "Something went terribly wrong patching memory. The game will probably crash or behave incorrectly.", PLUGIN_NAME_LONG, MB_OK | MB_ICONERROR);
			_MESSAGE("%s load failed epicly.", PLUGIN_NAME_LONG);
			return false;
		}
		_MESSAGE("%s load successful.", PLUGIN_NAME_LONG);
		return true;
	}
};
#endif /* _DXGI_SHIM */