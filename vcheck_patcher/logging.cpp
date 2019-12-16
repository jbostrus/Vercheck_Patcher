#if defined(F4SE) || defined(SKSE64)
#include "common/IPrefix.h"
#else
#include <string>
#endif /* F4SE || SKSE64 */
#include <shlobj.h>
#include "config.h"
#include "logging.h"


#if !defined(F4SE) && !defined(SKSE64)
// Logging
FILE* logfile = nullptr;

void _MESSAGE(const char* format, ...)
{
	static char outputBuf[8192];
	if (logfile) {
		va_list args; va_start(args, format);
		vsnprintf(outputBuf, sizeof(outputBuf), format, args);
		va_end(args);
		fprintf(logfile, "%s\n", outputBuf);
		fflush(logfile);
	}
}
#endif /* F4SE || SKSE64 */

void OpenLogFile()
{
#define LOG_BASE_PATH "\\My Games\\"
#ifdef _SKYRIM64
#define LOG_GAME_PATH "Skyrim Special Edition\\"
#elif _FALLOUT4
#define LOG_GAME_PATH "Fallout4\\"
#endif

#ifdef _DXGI_SHIM
#define LOG_EXT_PATH "Logs\\"
#elif F4SE
#define LOG_EXT_PATH "F4SE\\"
#elif SKSE64
#define LOG_EXT_PATH "SKSE\\"
#endif /* DXGI || F4SE || SKSE64 */

#define LOG_FULL_PATH LOG_BASE_PATH LOG_GAME_PATH LOG_EXT_PATH PLUGIN_LOGFILE_NAME

#if defined(F4SE) || defined(SKSE64)
	gLog.OpenRelative(CSIDL_MYDOCUMENTS, LOG_FULL_PATH);
#else
	// Logging
	char logPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS | CSIDL_FLAG_CREATE, NULL, NULL, logPath))) {
		snprintf(logPath, sizeof(logPath), "%s%s", logPath, LOG_FULL_PATH);
		logfile = _fsopen(logPath, "w", _SH_DENYWR);
	}
#endif /* F4SE || SKSE64 */
	_MESSAGE(PLUGIN_PRODUCT_NAME);
}

void CloseLogFile()
{
#if !defined(F4SE) && !defined(SKSE64)
	if (logfile)
		fclose(logfile);
#endif /* F4SE || SKSE64 */
}