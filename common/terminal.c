#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <wchar.h>
#include <ctype.h>
#include <locale.h>
#include <unistd.h>
#include "../glyph/cursor.h"
#include "terminal.h"

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
HANDLE hIn;
HANDLE hOut;
DWORD dwOriginalInMode;
DWORD dwOriginalOutMode;
volatile bool signal_close = false;
#else
#include <sys/ioctl.h>
#include <sys/select.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <errno.h>
struct termios orig_termios;
volatile sig_atomic_t signal_winch = false;
volatile sig_atomic_t signal_hup = false;
volatile sig_atomic_t signal_term = false;
volatile sig_atomic_t signal_cont = false;
#endif

int terminal_height;
int terminal_width;

static inline int len(unsigned long number){
	int i;
	for(i = 8 * sizeof(number) - 1 ; i >= 0 && !((number >> i) & 1) ; i--);
	return (i + 1) * 3 / 10 + 1;
}

#ifndef _WIN32
void handle_winch(int){
	signal_winch = true;
}

void handle_hup(int){
	signal_hup = true;
}

void handle_term(int){
	signal_term = true;
}

void handle_cont(int){
	signal_cont = true;
}
#endif

void clear(){
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

char get_key(){
#ifdef _WIN32
	if(_kbhit()) return _getch();
	else return 0;
#else
	char c;
	if(read(STDIN_FILENO, &c, 1) == 1) return c;
	return 0;
#endif
}

/*
  Encode rule:
  Using unsigned long(32 bits):
    MSB
    X01 X02 X03 X04 X05 X06 X07 X08 X09 X10 X11 X12 X13 X14 X15 X16
    X17 X18 X19 X20 X21 X22 X23 X24 X25 X26 X27 X28 X29 X30 X31 X32
                                                                LSB

   Bit     | Keyboard    | Mouse
  ---------+------------------------------
   X01     |            Ctrl
   X02     |           Shift
   X03     |          Meta(Alt)
   X04     |           Mouse
   X05     | Type_1      | Button_01
   X06     | Type_2      | Button_02
   X07     | Special_1   | Wheel
   X08     | Special_2   | Drag
   X09     | Special_3   | Click
   X10     | Special_4   | x1
   X11     | Special_5   | x2
   X12     | ASCII_1     | x3
   X13     | ASCII_2     | x4
   X14     | ASCII_3     | x5
   X15     | ASCII_4     | x6
   X16     | ASCII_5     | x7
   X17     | ASCII_6     | x8
   X18     | ASCII_7     | x9
   X19     | ASCII_8     | x10
   X20     | Error_1     | x11
   X21     | Error_2     | x12
   X22     | Error_3     | y1
   X23~X32 | Others      | y2~y11

  Type:
    ASCII    : 0
    Special  : 1
    Error    : 2
    Others   : 3

  Specials:
    F1 ~ F12              : 0 ~ 11
    Home/End              : 12, 13
    PgUp/PgDn             : 14, 15
    Insert/Delete         : 16, 17
    Up, Down, Left, Right : 18, 19, 20, 21
	Unicode               : 22 (ASCII, Error and Others will be occupied by unicode)

  Button:
       | Normal/Click  | Wheel
    ---+---------------+----------
     0 | Nothing       | Up
     1 | Left          | Down
     2 | Middle        | Left
     3 | Right         | Right

  Errors:
    0   : None
    1   : TLE
    2   : Unknown Key
    3~7 : Reserved

  Others:
    0      : Terminal Resized (SIGWINCH)
	1      : Terminal Destroyed (SIGHUP)
	2      : Terminal Killed (SIGTERM)
	3      : Terminal Continued (SIGCONT)
	4      : Program Shutdown
	5      : Program Restart
	6      : Program Initialization
	7      : Toggle Program Mouse Detection
	8      : Toggle Program Keyboard Detection
	9~1023 : Reserved
*/

/**
### Rules:
Home      : [H or [1~
End       : [F or [4~
PgUp      : [5~
PgDn      : [6~
Insert    : [2~
Delete    : [3~
Up        : [A
Down      : [B
Left      : [D
Right     : [C
C-<key>   : <key-64>
M-<key>   : \x1b<key>
C-M-<key> : \x1b<key-64>
F<num>    : OP, OQ, OR, OS, [15~, [17~, [18~, [19~, [20~, [21~, [23~, [24~
Mouse     : [<{button};x;y
---
### Combinations:
Modifier Codes:
Shift     : 2
M         : 3
Shift-M   : 4
C         : 5
Shift-C   : 6
M-C       : 7
Shift-M-C : 8
**/

unsigned long keyboard_escape_parser(unsigned char *string, int index){
	if(string == NULL) return 0;
	unsigned long result = 0;
	int i = 0, semi_column = -1;
	for(i = 0 ; i < index && semi_column == -1 ; i++)
		if(string[i] == ';') semi_column = i;

	// Single Alt(Meta) Key
	if(index == 1){
		SET_KEYBOARD_TYPE(result, 0);
		SET_ALT_KEY(result);
		SET_KEYBOARD_ASCII(result, string[0]);
		return result;
	}

	if(index == 2 && string[0] == '[' && string[1] == 'Z'){
		SET_KEYBOARD_TYPE(result, 0);
		SET_SHIFT_KEY(result);
		SET_KEYBOARD_ASCII(result, '\t');
		return result;
	}
	// Combinations
	int combination = -1;
	
	if(semi_column != -1 && semi_column + 1 < index) combination = (int)(string[semi_column + 1] - '0') - 1;
	if(combination < 1 || combination > 7) combination = -1;
	if(combination != -1){
		if(combination & 0b001) SET_SHIFT_KEY(result);
		if(combination & 0b010) SET_ALT_KEY(result);
		if(combination & 0b100) SET_CONTROL_KEY(result);
		for(i = semi_column + 2 ; i < index ; i++) string[i - 2] = string[i];
		index -= 2;
		string[index] = 0;
	}

	// Special Keys
	// F1 ~ F4
	SET_KEYBOARD_TYPE(result, 1);
	if(((index == 2 && string[0] == 'O') || (index == 3 && string[0] == '[' && string[1] == '1')) && (string[index - 1] >= 'P' && string[index - 1] <= 'S')){
		SET_KEYBOARD_SPECIAL(result, string[index - 1] - 'P');
		return result;
	}
	// F5 ~ F12
	if(index == 4 && string[0] == '[' && string[3] == '~' && string[1] >= '0' && string[1] <= '9' && string[2] >= '0' && string[2] <= '9'){
		if(string[1] == '1'){
			switch(string[2]){
			case '5': SET_KEYBOARD_SPECIAL(result, 4); return result;
			case '7': SET_KEYBOARD_SPECIAL(result, 5); return result;
			case '8': SET_KEYBOARD_SPECIAL(result, 6); return result;
			case '9': SET_KEYBOARD_SPECIAL(result, 7); return result;
			}
		}
		else if(string[1] == '2'){
			switch(string[2]){
			case '0': SET_KEYBOARD_SPECIAL(result, 8); return result;
			case '1': SET_KEYBOARD_SPECIAL(result, 9); return result;
			case '3': SET_KEYBOARD_SPECIAL(result, 10); return result;
			case '4': SET_KEYBOARD_SPECIAL(result, 11); return result;
			}
		}
	}

	// ARROW + HOME/END
	if((index == 2 || (index == 3 && string[1] == '1')) && string[0] == '['){
		switch(string[index - 1]){
			case 'A': SET_KEYBOARD_SPECIAL(result, 18); return result;
			case 'B': SET_KEYBOARD_SPECIAL(result, 19); return result;
			case 'C': SET_KEYBOARD_SPECIAL(result, 21); return result;
			case 'D': SET_KEYBOARD_SPECIAL(result, 20); return result;
			case 'F': SET_KEYBOARD_SPECIAL(result, 13); return result;
			case 'H': SET_KEYBOARD_SPECIAL(result, 12); return result;
		}
	}

	// HOME/END + INSERT/DELETE + PGUP/PGDN
	if(index == 3 && string[0] == '[' && string[2] == '~'){
		switch(string[1]){
			case '1': SET_KEYBOARD_SPECIAL(result, 12); return result;
			case '2': SET_KEYBOARD_SPECIAL(result, 16); return result;
			case '3': SET_KEYBOARD_SPECIAL(result, 17); return result;
			case '4': SET_KEYBOARD_SPECIAL(result, 13); return result;
			case '5': SET_KEYBOARD_SPECIAL(result, 14); return result;
			case '6': SET_KEYBOARD_SPECIAL(result, 15); return result;
		}
	}

	CLEAN_KEYBOARD_TYPE(result);
	SET_KEYBOARD_TYPE(result, 2);
	SET_KEYBOARD_ERROR(result, 2);
	return result;
}

unsigned long mouse_escape_parser(unsigned char *string){
/**
modifier:
  Bits  Usage
  0-1   Button/Direction(UP, DOWN, LEFT, RIGHT)
  2     Shift
  3     Alt
  4     Ctrl
  5     Motion
  6     Wheel
  7     Reserved
 **/
	
	if(string == NULL) return 0;
	unsigned int modifier, x, y;
	char pd;
	if(sscanf((char*)string, "[<%d;%d;%d%c", &modifier, &x, &y, &pd) != 4) return 0;
	if(pd != 'm' && pd != 'M') return 0;
	int result = 0;
	SET_MOUSE_KEY(result);
	if(modifier & (1 << 2)) SET_SHIFT_KEY(result);
	if(modifier & (1 << 3)) SET_ALT_KEY(result);
	if(modifier & (1 << 4)) SET_CONTROL_KEY(result);
	if(modifier & (1 << 6)){
		SET_MOUSE_WHEEL(result);
		SET_MOUSE_BUTTON(result, modifier & 0b11);
	}
	else if((modifier & (1 << 5)) && ((modifier & 0b11) != 3) && pd == 'M'){
		SET_MOUSE_DRAG(result);
		SET_MOUSE_BUTTON(result, (modifier + 1) & 0b11);
	}
	else{
		SET_MOUSE_BUTTON(result, (modifier + 1) & 0b11);
		if(pd == 'M' && (modifier & 0b11) != 3) SET_MOUSE_CLICK(result);
	}
	SET_MOUSE_X(result, x);
	SET_MOUSE_Y(result, y);
	return result;
}

void toUnicode(unsigned long code, unsigned char *buffer){
	code = GET_KEYBOARD_UNICODE(code);
	if(code < 0x80){
		buffer[0] = (unsigned char)code;
		buffer[1] = 0;
	}
	else if(code < 0x800){
		buffer[0] = 0xC0 | (code >> 6);
		buffer[1] = 0x80 | (code & 0x3F);
		buffer[2] = 0;
	}
	else if(code < 0x10000){
		buffer[0] = 0xE0 | (code >> 12);
		buffer[1] = 0x80 | ((code >> 6) & 0x3F);
		buffer[2] = 0x80 | (code & 0x3F);
		buffer[3] = 0;
	}
	else if(code < 0x110000){
		buffer[0] = 0xF0 | (code >> 18);
		buffer[1] = 0x80 | ((code >> 12) & 0x3F);
		buffer[2] = 0x80 | ((code >> 6) & 0x3F);
		buffer[3] = 0x80 | (code & 0x3F);
		buffer[4] = 0;
	}
	else
		buffer[0] = 0;
}

unsigned long keyboard_unicode(const unsigned char *string){
	unsigned long result = 0;
	unsigned int unicode = 0;
	int i, len;

	if(string == NULL || string[0] == 0) return 0;

	if((string[0] & 0x80) == 0){
		len = 1;
		unicode = string[0];
	}
	else if((string[0] & 0xE0) == 0xC0){
		len = 2;
		unicode = string[0] & 0x1F;
	}
	else if((string[0] & 0xF0) == 0xE0){
		len = 3;
		unicode = string[0] & 0x0F;
	}
	else if((string[0] & 0xF8) == 0xF0){
		len = 4;
		unicode = string[0] & 0x07;
	}
	else len = 0;

	for(i = 1 ; i < len ; i++){
		if((string[i] & 0xC0) != 0x80){
			unicode = 0;
			break;
		}
		unicode = (unicode << 6) | (string[i] & 0x3F);
	}

	
	SET_KEYBOARD_TYPE(result, 1);
	SET_KEYBOARD_SPECIAL(result, 22);
	SET_KEYBOARD_UNICODE(result, unicode);
	return result;
}

#ifdef _WIN32
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType){
	switch(fdwCtrlType){
	case CTRL_C_EVENT: return TRUE;
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		Sleep(500);
		signal_close = true;
		return TRUE;
	default: return FALSE;
	}
}

static inline bool Windows_Resize(INPUT_RECORD *record){
	return record->EventType == WINDOW_BUFFER_SIZE_EVENT;
}

static inline bool Windows_Ctrl(INPUT_RECORD *record){
	return ((record->Event.KeyEvent.dwControlKeyState | record->Event.MouseEvent.dwControlKeyState) & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED));
}

static inline bool Windows_Shift(INPUT_RECORD *record){
	return ((record->Event.KeyEvent.dwControlKeyState | record->Event.MouseEvent.dwControlKeyState) & SHIFT_PRESSED);
}

static inline bool Windows_Alt(INPUT_RECORD *record){
	return ((record->Event.KeyEvent.dwControlKeyState | record->Event.MouseEvent.dwControlKeyState) & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED));
}

static inline bool Windows_Mouse(INPUT_RECORD *record){
	return record->EventType == MOUSE_EVENT;
}

static inline bool Windows_Keyboard(INPUT_RECORD *record){
	return record->EventType == KEY_EVENT && record->Event.KeyEvent.bKeyDown;
}

static inline bool Windows_Wheel(INPUT_RECORD *record){
	return record->Event.MouseEvent.dwEventFlags & MOUSE_WHEELED;
}

static inline bool Windows_Drag(INPUT_RECORD *record){
	return (record->Event.MouseEvent.dwEventFlags & MOUSE_MOVED) && (record->Event.MouseEvent.dwButtonState != 0);
}

static inline bool Windows_Click(INPUT_RECORD *record){
	return record->Event.MouseEvent.dwEventFlags == 0;
}

static inline unsigned int Windows_Button(INPUT_RECORD *record){
	if(Windows_Wheel(record))
		return ((record->Event.MouseEvent.dwButtonState >> 16) <= 0);
	unsigned int temp = record->Event.MouseEvent.dwButtonState & 0b111;
	return (temp & 0b001) + 2 * ((temp & 0b100) >> 2) + 3 * ((temp & 0b010) >> 1);
}

static inline unsigned int Windows_MouseX(INPUT_RECORD *record){
	return record->Event.MouseEvent.dwMousePosition.X + 1;
}

static inline unsigned int Windows_MouseY(INPUT_RECORD *record){
	return record->Event.MouseEvent.dwMousePosition.Y + 1;
}

static inline unsigned char Windows_Keyboard_Get(INPUT_RECORD *record){
	return record->Event.KeyEvent.uChar.AsciiChar;
}

unsigned long Windows_get_parser(){
	if(signal_close){
		signal_close = false;
		unsigned long result = 0;
		SET_KEYBOARD_TYPE(result, 3);
		SET_KEYBOARD_OTHERS(result, 1);
		return result;
	}
	
	static INPUT_RECORD record_buffer[128];
	static DWORD records_in_buffer = 0;
	static DWORD current_record_index = 0;

	while(1){
		while(current_record_index < records_in_buffer){
			INPUT_RECORD* record = &record_buffer[current_record_index++];
			
			unsigned long result = 0;
			if(Windows_Resize(record)){
				set_term_size();
				SET_KEYBOARD_TYPE(result, 3);
				SET_KEYBOARD_OTHERS(result, 0);
				return result;
			}
			else if(Windows_Mouse(record)){
				if(record->Event.MouseEvent.dwEventFlags != MOUSE_MOVED && record->Event.MouseEvent.dwButtonState != 0) continue;
				SET_MOUSE_KEY(result);
				if(Windows_Ctrl(record)) SET_CONTROL_KEY(result);
				if(Windows_Shift(record)) SET_SHIFT_KEY(result);
				if(Windows_Alt(record)) SET_ALT_KEY(result);
				if(Windows_Wheel(record)) SET_MOUSE_WHEEL(result);
				if(Windows_Drag(record)) SET_MOUSE_DRAG(result);
				if(Windows_Click(record)) SET_MOUSE_CLICK(result);
				SET_MOUSE_BUTTON(result, Windows_Button(record));
				SET_MOUSE_X(result, Windows_MouseX(record));
				SET_MOUSE_Y(result, Windows_MouseY(record));
				return result;
			}
			else if(Windows_Keyboard(record)){
				unsigned char c = Windows_Keyboard_Get(record);
				if(c == '\x1b'){
					unsigned char str[32] = {0};
					int index = 0;
					str[index++] = '\x1b';
					
					while(current_record_index < records_in_buffer){
						INPUT_RECORD* next_record = &record_buffer[current_record_index];
						if(Windows_Keyboard(next_record)){
							str[index++] = Windows_Keyboard_Get(next_record);
							current_record_index++;
						}
						else break;
					}
					
					if(index == 0) return 0;
					else if(index == 1){
						SET_KEYBOARD_TYPE(result, 0);
						SET_KEYBOARD_ASCII(result, '\x1b');
						return result;
					}
					else if(index > 3 && str[1] == '[' && str[2] == '<') return mouse_escape_parser(str + 1);
					return keyboard_escape_parser(str + 1, index - 1);
				}
				else if(c > 0x7F){
					unsigned char str[32] = {0};
					int index = 0;
					str[index++] = c;
					
					while(current_record_index < records_in_buffer){
						INPUT_RECORD* next_record = &record_buffer[current_record_index];
						if(Windows_Keyboard(next_record)){
							str[index++] = Windows_Keyboard_Get(next_record);
							current_record_index++;
						}
						else break;
					}
					return keyboard_unicode(str);
				}
				else if(c != 0)
					return keyboard_stimulator(Windows_Ctrl(record), Windows_Shift(record), Windows_Alt(record), 0, c);
			}
		}
		DWORD events_available;
		if(!ReadConsoleInput(hIn, record_buffer, 128, &records_in_buffer)) return 0;
		
		current_record_index = 0;
	}
}
#endif

unsigned long get(){
#ifdef _WIN32
	return Windows_get_parser();
#else
	static unsigned char buffer[128] = {0};
	static int current = 0;
	static int stack = 0;
	unsigned long result = 0;

	while(1){
		if(stack > current){
			unsigned char* ptr = buffer + current;
			int index = stack - current;
			if(index > 1 && *ptr > 0x7F){
				unsigned char temp = *ptr;
				result = keyboard_unicode(buffer + current);
				if((temp & 0xE0) == 0xC0) current += 2;
				else if((temp & 0xF0) == 0xE0) current += 3;
				else if((temp & 0xF8) == 0xF0) current += 4;
				else current++;
				return result;
			}
			else if(index > 1 && ptr[0] == '\x1b'){
				if((index > 3 && ptr[1] == '[' && ptr[2] == '<')){
					result = mouse_escape_parser(ptr + 1);
					int i, semi_column = 0;
					for(i = 2 ; i < index && ptr[i] != 0 ; i++){
						if(semi_column < 3 && ptr[i] == ';') semi_column++;
						else if(semi_column == 2 && (ptr[i] == 'm' || ptr[i] == 'M'))
							break;
					}
					if(ptr[i] == 'm' || ptr[i] == 'M') current += i + 1;
					else{
						result = 0;
						current++;
						SET_KEYBOARD_TYPE(result, 0);
						SET_KEYBOARD_ASCII(result, '\x1b');
					}
					return result;
				}
				else{
					result = keyboard_escape_parser(ptr + 1, index - 1);
					char* temp = escape_sequence(result);
					
					if(temp == NULL){
						current++;
						SET_KEYBOARD_TYPE(result, 0);
						SET_KEYBOARD_ASCII(result, '\x1b');
						return result;
					}
					current += strlen(temp) + 1;
					free(temp);
					return result;
				}
			}
			else{
				SET_KEYBOARD_TYPE(result, 0);
				SET_KEYBOARD_ASCII(result, *ptr);
				current++;
				return result;
			}
		}
		else{
			if(signal_winch){
				signal_winch = false;
				set_term_size();
				SET_KEYBOARD_TYPE(result, 3);
				SET_KEYBOARD_OTHERS(result, 0);
				return result;
			}
			if(signal_hup){
				signal_hup = false;
				SET_KEYBOARD_TYPE(result, 3);
				SET_KEYBOARD_OTHERS(result, 1);
				return result;
			}
			if(signal_term){
				signal_term = false;
				SET_KEYBOARD_TYPE(result, 3);
				SET_KEYBOARD_OTHERS(result, 2);
				return result;
			}
			if(signal_cont){
				signal_cont = false;
				SET_KEYBOARD_TYPE(result, 3);
				SET_KEYBOARD_OTHERS(result, 3);
				return result;
			}
			
			int retval;
			fd_set rfds;
			struct timeval tv;
			
			FD_ZERO(&rfds);
			FD_SET(STDIN_FILENO, &rfds);

			tv.tv_sec = 0;
			tv.tv_usec = 20000;
			
			retval = select(STDIN_FILENO + 1, &rfds, NULL, NULL, &tv);
			if(retval == -1){
				stack = 0;
				return 0;
			}
			else if(retval){
				stack = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
				if(stack <= 0) return 0;
				buffer[stack] = 0;
				current = 0;
			}
			else return 0;
		}
	}
#endif
}

unsigned long keyboard_stimulator(bool ctrl, bool shift, bool alt, char type, unsigned long content){
	unsigned int result = 0;
	SET_KEYBOARD_TYPE(result, type);
	if(type == 0){
		if(ctrl && isalpha(content)) content = toupper(content) - 64;
		SET_KEYBOARD_ASCII(result, content);
		if(alt) SET_ALT_KEY(result);
		return result;
	}
	else if(type == 1){
		if(ctrl) SET_CONTROL_KEY(result);
		if(shift) SET_SHIFT_KEY(result);
		if(alt) SET_ALT_KEY(result);
		SET_KEYBOARD_SPECIAL(result, content);
	}
	else if(type == 2)
		SET_KEYBOARD_ERROR(result, content);
	return result;
}

unsigned long mouse_stimulator(bool ctrl, bool shift, bool alt, char button, bool wheel, bool drag, bool click, int x, int y){
	unsigned int result = 0;
	SET_MOUSE_KEY(result);
	if(ctrl) SET_CONTROL_KEY(result);
	if(shift) SET_SHIFT_KEY(result);
	if(alt) SET_ALT_KEY(result);
	SET_MOUSE_BUTTON(result, button & 0b11);
	if(wheel) SET_MOUSE_WHEEL(result);
	else if(drag) SET_MOUSE_DRAG(result);
	if(click) SET_MOUSE_CLICK(result);
	SET_MOUSE_X(result, x & 0b111111111111);
	SET_MOUSE_Y(result, y & 0b11111111111);
	return result;
}

char* escape_sequence(unsigned long code){
	bool mouse = GET_MOUSE_KEY(code);
	bool ctrl = GET_CONTROL_KEY(code);
	bool shift = GET_SHIFT_KEY(code);
	bool alt = GET_ALT_KEY(code);
	if(mouse){
		char button = GET_MOUSE_BUTTON(code), click = (GET_MOUSE_CLICK(code)) ? 'M' : 'm';
		bool wheel = GET_MOUSE_WHEEL(code), drag = GET_MOUSE_DRAG(code);
		int modifier = (wheel << 6) | (drag << 5) | (ctrl << 4) | (shift << 3) | (alt << 2) | button;
		int x = GET_MOUSE_X(code), y = GET_MOUSE_Y(code);
		char *result = malloc(len(x) + len(y) + len(modifier) + 7);
		if(result == NULL) return NULL;
		sprintf(result, "\x1b[<%d;%d;%d%c", modifier, x, y, click);
		return result;
	}
	else{
		char type = GET_KEYBOARD_TYPE(code), content = 0;
		char *result = malloc(32);
		if(result == NULL) return NULL;
		memset(result, 0, 32);
		if(type == 0){
			int i = 0;
			content = GET_KEYBOARD_ASCII(code);
			if(alt)	result[i++] = '\x1b';
			if(shift) content = toupper(content);
			if(shift && content == '\t'){
				sprintf(result, "\x1b[Z");
				return result;
			}
			result[i++] = content;
		}
		else if(type == 1){
			content = GET_KEYBOARD_SPECIAL(code);
			int combination = 1 + (shift) + (alt << 1) + (ctrl << 2);
			// F1 ~ F4
			if(content >= 0 && content <= 3){
				if(combination - 1)
					sprintf(result, "\x1b[1;%d%c", combination, 'P' + content);
				else
					sprintf(result, "\x1bO%c", 'P' + content);
			}
			// F5 ~ F12
			else if(content >= 4 && content <= 11){
				int temp[8] = {15, 17, 18, 19, 20, 21, 23, 24};
				if(combination - 1)
					sprintf(result, "\x1b[%d;%d~", temp[content - 4], combination);
				else
					sprintf(result, "\x1b[%d~", temp[content - 4]);
			}
			else if(content < 22){
				static char arr[10] = {'H', 'F', '5', '6', '2', '3', 'A', 'B', 'D', 'C'};
				if(isalpha(arr[content - 12])){
					sprintf(result, "\x1b");
					if(combination - 1) sprintf(result + 1, "[1;%d", combination);
					int temp = strlen(result);
					result[temp] = arr[content - 12];
					result[temp + 1] = 0;
				}
				else{
					sprintf(result, "\x1b[%c", arr[content - 12]);
					if(combination - 1) sprintf(result + strlen(result), ";%d", combination);
					strcat(result, "~");
				}
			}
		}
		else if(type == 2){
			content = GET_KEYBOARD_ERROR(code);
			if(content == 1) sprintf(result, "TLE");
			else if(content == 2) sprintf(result, "Unknown Sequence");
		}
		else if(type == 3){
			free(result);
			result = NULL;
			return NULL;
		}
		if(result[0] == 0){
			free(result);
			result = NULL;
			return NULL;
		}
		else return result;
	}
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
	printf("\x1b[?1003l\x1b[?1006l");
    printf(_cursor_show);
    fflush(stdout);
	SetConsoleMode(hIn, dwOriginalInMode);
	SetConsoleMode(hOut, dwOriginalOutMode);
#else
	printf("\x1b[?1003l\x1b[?1006l");
    printf(_cursor_show);
    fflush(stdout);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
#endif
}

void enableRawMode(){
#ifdef _WIN32
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	setlocale(LC_ALL, ".UTF8");
	hIn = GetStdHandle(STD_INPUT_HANDLE);
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	if(hIn == INVALID_HANDLE_VALUE || hOut == INVALID_HANDLE_VALUE) exit(1);

	if(!GetConsoleMode(hIn, &dwOriginalInMode)) exit(1);
	if(!GetConsoleMode(hOut, &dwOriginalOutMode)) exit(1);

	DWORD dwInMode = dwOriginalInMode;
	dwInMode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
	dwInMode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);

	DWORD dwOutMode = dwOriginalOutMode;
	dwOutMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

	if(!SetConsoleMode(hIn, dwInMode)) exit(1);
	if(!SetConsoleMode(hOut, dwOutMode)) exit(1);
#else
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
#endif
    atexit(disableRawMode);
	printf("\x1b[?1003h\x1b[?1006h");
    printf(_cursor_hide);
    fflush(stdout);
}

void WINterm_mode(){
#ifdef _WIN32
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	setlocale(LC_ALL, ".UTF8");
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	if(hOut == INVALID_HANDLE_VALUE) exit(1);

	if(!GetConsoleMode(hOut, &dwOriginalOutMode)) exit(1);

	DWORD dwOutMode = dwOriginalOutMode;
	dwOutMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

	if(!SetConsoleMode(hOut, dwOutMode)) exit(1);
#endif
}

void TUI_mode(){
	set_term_size();
	clear();
#ifdef _WIN32
	if(!SetConsoleCtrlHandler(CtrlHandler, TRUE)) exit(1);
#else
	setlocale(LC_ALL, "");
	signal(SIGWINCH, handle_winch);
	signal(SIGHUP, handle_hup);
	signal(SIGTERM, handle_term);
	signal(SIGCONT, handle_cont);
	signal(SIGPIPE, SIG_IGN);
#endif
	enableRawMode();
}
