#ifndef WINDOWS_TERMINAL_MODE_FOR_UNICODE_H
#define WINDOWS_TERMINAL_MODE_FOR_UNICODE_H

#ifdef __cplusplus
extern "C"{
#endif

#ifdef _WIN32
#include <windows.h>
#endif

inline void win_term_init() {
#ifdef _WIN32
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) {
		return;
	}
	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode)) {
		return;
	}
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hOut, dwMode)) {
		return;
	}
#endif
}

#ifdef __cplusplus
}
#endif

#endif
