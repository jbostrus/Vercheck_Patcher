#ifndef __LOGGING_H__
#define __LOGGING_H__

void OpenLogFile();
void CloseLogFile();

#if !defined(F4SE) && !defined(SKSE64)
void _MESSAGE(const char* format, ...);
#endif

#endif