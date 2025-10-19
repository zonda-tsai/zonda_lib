#ifndef TERMINAL_BRANCH
#define TERMINAL_BRANCH

#ifdef __cplusplus
extern "C"{
#endif

#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
extern HANDLE hIn;
extern HANDLE hOut;
extern DWORD dwOriginalInMode;
extern DWORD dwOriginalOutMode;
#else
extern struct termios orig_termios;
#endif
extern int terminal_height, terminal_width;

#define CLEAN_CONTROL_KEY(value) ((value) &= ~(1 << 31))
#define CLEAN_SHIFT_KEY(value) ((value) &= ~(1 << 30))
#define CLEAN_ALT_KEY(value) ((value) &= ~(1 << 29))
#define CLEAN_MOUSE_KEY(value) ((value) &= ~(1 << 28))
#define SET_CONTROL_KEY(value) ((value) |= (1 << 31))
#define SET_SHIFT_KEY(value) ((value) |= (1 << 30))
#define SET_ALT_KEY(value) ((value) |= (1 << 29))
#define SET_MOUSE_KEY(value) ((value) |= (1 << 28))
#define GET_CONTROL_KEY(value) (((value) >> 31) & 1)
#define GET_SHIFT_KEY(value) (((value) >> 30) & 1)
#define GET_ALT_KEY(value) (((value) >> 29) & 1)
#define GET_MOUSE_KEY(value) (((value) >> 28) & 1)

#define CLEAN_KEYBOARD_TYPE(value) ((value) &= ~(0b11 << 26))
#define SET_KEYBOARD_TYPE(value, type) ((value) |= (((type) & 0b11) << 26))
#define GET_KEYBOARD_TYPE(value) (((value) >> 26) & 0b11)

#define CLEAN_KEYBOARD_SPECIAL(value) ((value) &= ~(0b11111 << 21))
#define CLEAN_KEYBOARD_ASCII(value) ((value) &= ~(0b11111111 << 13))
#define CLEAN_KEYBOARD_ERROR(value) ((value) &= ~(0b111 << 10))
#define CLEAN_KEYBOARD_OTHERS(value) ((value) &= (~0b1111111111))
#define CLEAN_KEYBOARD_UNICODE(value) ((value) &= (~0x1FFFFF))
#define SET_KEYBOARD_SPECIAL(value, special) ((value) |= (((special) & 0b11111) << 21))
#define SET_KEYBOARD_ASCII(value, ascii) ((value) |= (((ascii) & 0b11111111) << 13))
#define SET_KEYBOARD_ERROR(value, error) ((value) |= (((error) & 0b111) << 10))
#define SET_KEYBOARD_OTHERS(value, others) ((value) |= (((others) & 0b1111111111)))
#define SET_KEYBOARD_UNICODE(value, unicode) ((value) |= (((unicode) & 0x1FFFFF)))
#define GET_KEYBOARD_SPECIAL(value) (((value) >> 21) & 0b11111)
#define GET_KEYBOARD_ASCII(value) (((value) >> 13) & 0b11111111)
#define GET_KEYBOARD_ERROR(value) (((value) >> 10) & 0b111)
#define GET_KEYBOARD_OTHERS(value) ((value) & 0b1111111111)
#define GET_KEYBOARD_UNICODE(value) ((value) & 0x1FFFFF)

#define CLEAN_MOUSE_BUTTON(value) ((value) &= ~(0b11 << 26))
#define CLEAN_MOUSE_WHEEL(value) ((value) &= ~(1 << 25))
#define CLEAN_MOUSE_DRAG(value) ((value) &= ~(1 << 24))
#define CLEAN_MOUSE_CLICK(value) ((value) &= ~(1 << 23))
#define CLEAN_MOUSE_X(value) ((value) &= ~(0b111111111111 << 11))
#define CLEAN_MOUSE_Y(value) ((value) &= ~(0b11111111111))
#define SET_MOUSE_BUTTON(value, mod) ((value) |= ((((mod) & 0b11) << 26)))
#define SET_MOUSE_WHEEL(value) ((value) |= (1 << 25))
#define SET_MOUSE_DRAG(value) ((value) |= (1 << 24))
#define SET_MOUSE_CLICK(value) ((value) |= (1 << 23))
#define SET_MOUSE_X(value, x) ((value) |= (((x) & 0b111111111111) << 11))
#define SET_MOUSE_Y(value, y) ((value) |= ((y) & 0b11111111111))
#define GET_MOUSE_BUTTON(value) (((value) >> 26) & 0b11)
#define GET_MOUSE_WHEEL(value) (((value) >> 25) & 1)
#define GET_MOUSE_DRAG(value) (((value) >> 24) & 1)
#define GET_MOUSE_CLICK(value) (((value) >> 23) & 1)
#define GET_MOUSE_X(value) (((value) >> 11) & 0b111111111111)
#define GET_MOUSE_Y(value) ((value) & 0b11111111111)

#ifdef _WIN32
BOOL WINAPI CtrlHandler(DWORD);
static inline bool Windows_Resize(INPUT_RECORD*);
static inline bool Windows_Ctrl(INPUT_RECORD*);
static inline bool Windows_Shift(INPUT_RECORD*);
static inline bool Windows_Alt(INPUT_RECORD*);
static inline bool Windows_Mouse(INPUT_RECORD*);
static inline bool Windows_Keyboard(INPUT_RECORD*);
static inline bool Windows_Wheel(INPUT_RECORD*);
static inline bool Windows_Drag(INPUT_RECORD*);
static inline bool Windows_Click(INPUT_RECORD*);
static inline unsigned int Windows_Button(INPUT_RECORD*);
static inline unsigned int Windows_MouseX(INPUT_RECORD*);
static inline unsigned int Windows_MouseY(INPUT_RECORD*);
static inline unsigned char Windows_Keyboard_Get(INPUT_RECORD*);
unsigned long Windows_get_parser();
#endif

void clear();
char get_key();
unsigned long keyboard_escape_parser(unsigned char*, int);
unsigned long mouse_escape_parser(unsigned char*);
void toUnicode(unsigned long, unsigned char*);
unsigned long keyboard_unicode(const unsigned char*);
unsigned long get();
unsigned long keyboard_stimulator(bool, bool, bool, char, unsigned long);
unsigned long mouse_stimulator(bool, bool, bool, char, bool, bool, bool, int, int);
char* escape_sequence(unsigned long);
void set_term_size();
void disableRawMode();
void enableRawMode();
void WINterm_mode();
void TUI_mode();

#ifdef __cplusplus
}
#endif

#endif
