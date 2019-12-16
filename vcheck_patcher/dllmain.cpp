#ifndef _DXGI_SHIM	// disable this code if we're building dxgi loader
#if defined(F4SE) || defined(SKSE64)
#include "common/IPrefix.h"
#endif /* F4SE | SKSE64 */
#ifdef F4SE
#include "f4se/PluginAPI.h"
#include "f4se_common/f4se_version.h"
#endif /* F4SE */
#ifdef SKSE64
#include "skse64/PluginAPI.h"
#include "skse64_common/skse_version.h"
#endif /* SKSE64 */
#include "config.h"
#include "logging.h"
#include "patcher.h"

#define VERSION_TO_STRING(a) std::to_string(GET_EXE_VERSION_MAJOR(a)) + "." + std::to_string(GET_EXE_VERSION_MINOR(a)) + "." + std::to_string(GET_EXE_VERSION_BUILD(a)) + (GET_EXE_VERSION_SUB(a) > 0? "." + GET_EXE_VERSION_SUB(a): "");

#if defined(F4SE) || defined(SKSE64)
IDebugLog gLog;
#endif /* F4SE | SKSE64 */

PluginHandle g_pluginHandle = kPluginHandle_Invalid;
const UInt32 pluginVersion = MAKE_EXE_VERSION_EX(PLUGIN_VERSION_INTEGER, PLUGIN_VERSION_INTEGER_MINOR, PLUGIN_VERSION_INTEGER_BETA, 0);
const std::string pluginVersionString = VERSION_TO_STRING(pluginVersion);

void OpenLogFile();		// from patcher.cpp

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		OpenLogFile();
		_MESSAGE("%s Logfile Opened.", PLUGIN_NAME_LONG);
		break;

	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C" {
#ifdef F4SE
	__declspec(dllexport) bool F4SEPlugin_Query(const F4SEInterface* f4se, PluginInfo* info)
	{
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
#endif /* F4SE */
#ifdef SKSE64
	__declspec(dllexport) bool SKSEPlugin_Query(const SKSEInterface* skse, PluginInfo* info)
	{
		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = PLUGIN_NAME_SHORT;
		info->version = pluginVersion;

		if (skse->isEditor) {
			_MESSAGE("loaded in editor, marking as incompatible");
			return false;
		}

		// supported runtime version
		_MESSAGE("%s query successful.", PLUGIN_NAME_LONG);
		return true;
	}

	__declspec(dllexport) bool SKSEPlugin_Load(const SKSEInterface* skse)
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
#endif /* SKSE64 */
};

#endif /* _DXGI_SHIM */