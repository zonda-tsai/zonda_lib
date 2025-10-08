#ifndef TERMINAL_BRANCH
#define TERMINAL_BRANCH

#ifdef __cplusplus
extern "C"{
#endif

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

#ifdef __cplusplus
}
#endif

#endif
