#ifndef TEXT_DECORATOR_H
#define TEXT_DECORATOR_H

#ifdef __cplusplus
extern "C"{
#endif

#include "win_term_mode.h"
	
#define _red "\x1b[31m"
#define _green "\x1b[32m"
#define _yellow "\x1b[33m"
#define _blue "\x1b[34m"
#define _violet "\x1b[35m"
#define _cyan "\x1b[36m"
#define _white "\x1b[37m"
#define _black "\x1b[30m"
#define _lightred "\x1b[91m"
#define _lightgreen "\x1b[92m"
#define _lightyellow "\x1b[93m"
#define _lightblue "\x1b[94m"
#define _lightviolet "\x1b[95m"
#define _lightcyan "\x1b[96m"
#define _lightwhite "\x1b[97m"
#define _lightblack "\x1b[90m"

#define _b_red "\x1b[41m"
#define _b_green "\x1b[42m"
#define _b_yellow "\x1b[43m"
#define _b_blue "\x1b[44m"
#define _b_violet "\x1b[45m"
#define _b_cyan "\x1b[46m"
#define _b_white "\x1b[47m"
#define _b_black "\x1b[40m"
#define _b_lightred "\x1b[101m"
#define _b_lightgreen "\x1b[102m"
#define _b_lightyellow "\x1b[103m"
#define _b_lightblue "\x1b[104m"
#define _b_lightviolet "\x1b[105m"
#define _b_lightcyan "\x1b[106m"
#define _b_lightwhite "\x1b[107m"
#define _b_lightblack "\x1b[100m"

#define _true_color(stream, n) fprintf(stream, "\x1b[38;5;%dm", n)
#define _RGB(stream, r, g, b) fprintf(stream, "\x1b[38;2;%d;%d;%dm", r, g, b) 
#define _HEX(stream, rgb) _RGB(stream, rgb / 256 / 256, (rgb / 256) % 256, rgb % 256)
#define _b_true_color(stream, n) fprintf(stream, "\x1b[48;5;%dm", n)
#define _b_RGB(stream, r, g, b) fprintf(stream, "\x1b[48;2;%d;%d;%dm", r, g, b)
#define _b_HEX(stream, rgb) _b_RGB(stream, rgb / 256 / 256, (rgb / 256) % 256, rgb % 256)
#define _bold "\x1b[1m"
#define _italic "\x1b[3m"
#define _underline "\x1b[4m"
#define _strikeline "\x1b[9m"
#define _reverse "\x1b[7m"
#define _cancel_foreground "\x1b[39m"
#define _cancel_background "\x1b[49m"
#define _cancel_bold "\x1b[22m"
#define _cancel_italic "\x1b[23m"
#define _cancel_underline "\x1b[24m"
#define _cancel_strikeline "\x1b[29m"
#define _end "\x1b[0m"

#ifdef __cplusplus
}
#endif

#endif
