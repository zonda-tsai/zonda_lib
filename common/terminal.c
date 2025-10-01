#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include "../glyph/cursor.h"
#include "terminal.h"

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
HANDLE hStdin_win;
DWORD dwOriginalMode;
#else
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
struct termios orig_termios;
#endif

int terminal_height;
int terminal_width;

void clear(){
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

char get(){
#ifdef _WIN32
	return _getch();
#else
	char c;
	ssize_t n = read(STDIN_FILENO, &c, 1);
	if(n <= 0){
		fprintf(stderr, "Failed to use terminal.c/get() function...\n");
		exit(1);
	}
	return c;
#endif
}

void set_term_size(){
#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	terminal_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	terminal_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
	struct winsize ws;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	terminal_width = ws.ws_col;
	terminal_height = ws.ws_row;
#endif
}

void disableRawMode(){
#ifdef _WIN32
    SetConsoleMode(hStdin_win, dwOriginalMode);
#else
    printf(_cursor_show);
    fflush(stdout);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
#endif
}

void enableRawMode(){
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    win_term_init();
    hStdin_win = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin_win == INVALID_HANDLE_VALUE) exit(1);
    if (!GetConsoleMode(hStdin_win, &dwOriginalMode)) exit(1);
    DWORD dwNewMode = dwOriginalMode;
    dwNewMode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
    if (!SetConsoleMode(hStdin_win, dwNewMode)) exit(1);
#else
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
#endif
    atexit(disableRawMode);
    printf(_cursor_hide);
    fflush(stdout);
}

void TUI_mode(){
	set_term_size();
	clear();
#ifdef _WIN32
	setlocale(LC_ALL, "UTF-8");
#else
	setlocale(LC_ALL, "");
#endif
	enableRawMode();
}
