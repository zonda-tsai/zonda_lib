#ifndef TERMINAL_BRANCH
#define TERMINAL_BRANCH

#ifdef _WIN32
extern HANDLE hstdin_win;
extern DWORD dwOriginalMode;
#else
extern struct termios orig_termios;
#endif
extern int terminal_height, terminal_width;

void clear();
char get();
void set_term_size();
void disableRawMode();
void enableRawMode();
void TUI_mode();

#endif
