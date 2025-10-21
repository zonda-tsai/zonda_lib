#ifndef TEXT_DECORATOR_H
#define TEXT_DECORATOR_H

#ifdef __cplusplus
extern "C"{
#endif
	
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
#define _underline_color(stream, r, g, b) fprintf(stdout, "\x1b[58m;5;%d;%d;%dm", r, g, b);
#define _overline_color(stream, r, g, b) fprintf(stdout, "\x1b[59m;5;%d;%d;%dm", r, g, b);

#define _zonda_warn "\x1b[38;2;158;81;73m"
#define _zonda_b_warn "\x1b[48;2;158;81;73m"
#define _zonda_main "\x1b[38;2;57;58;87m"
#define _zonda_b_main "\x1b[48;2;57;58;87m"
#define _zonda_lightmain "\x1b[38;2;101;107;153m"
#define _zonda_b_lightmain "\x1b[48;2;101;107;153m"
#define _zonda_sub "\x1b[38;2;59;74;61m"
#define _zonda_b_sub "\x1b[48;2;59;74;61m"
#define _zonda_lightsub "\x1b[38;2;81;94;82m"
#define _zonda_b_lightsub "\x1b[48;2;81;94;82m"
#define _zonda_annotation "\x1b[38;2;192;192;192m"
#define _zonda_b_annotation "\x1b[48;2;64;64;64m"
#define _zonda_highlight "\x1b[38;2;175;95;0m"
#define _zonda_b_highlight "\x1b[48;2;175;95;0m"
#define _zonda_gold "\x1b[38;2;186;156;108m"
#define _zonda_b_gold "\x1b[48;2;186;156;108m"

#ifdef _HEX
#undef _HEX
#define _HEX(stream, rgb) _RGB(stream, rgb / 256 / 256, (rgb / 256) % 256, rgb % 256)
#endif

#define _b_true_color(stream, n) fprintf(stream, "\x1b[48;5;%dm", n)
#define _b_RGB(stream, r, g, b) fprintf(stream, "\x1b[48;2;%d;%d;%dm", r, g, b)
#define _b_HEX(stream, rgb) _b_RGB(stream, rgb / 256 / 256, (rgb / 256) % 256, rgb % 256)
#define _bold "\x1b[1m"
#define _italic "\x1b[3m"
#define _underline "\x1b[4m"
#define _underline_double "\x1b[4:2m"
#define _underline_wave "\x1b[4:3m"
#define _strikeline "\x1b[9m"
#define _overline "\x1b[53m"
#define _faint "\x1b[2m"
#define _sparkle "\x1b[5m"
#define _reverse "\x1b[7m"
#define _cancel_foreground "\x1b[39m"
#define _cancel_background "\x1b[49m"
#define _cancel_bold "\x1b[22m"
#define _cancel_italic "\x1b[23m"
#define _cancel_underline "\x1b[24m"
#define _cancel_strikeline "\x1b[29m"
#define _cancel_overline "\x1b[55m"
#define _cancel_faint "\x1b[22m"
#define _cancel_sparkle "\x1b[25m"
#define _end "\x1b[0m"

#define _sixel_start "\x1bPq"
#define _sixel_define_color_hls(stream, ID, h, l, s) fprintf(stream, "#%d;1;%d;%d;%d", ID, (int)(r / 360), l, s)
#define _sixel_define_color_rgb(stream, ID, r, g, b) fprintf(stream, "#%d;2;%d;%d;%d", ID, (int)(r / 2.55), (int)(g / 2.55), (int)(b / 2.55))
#define _sixel_using_color(stream, ID) fprintf(stream, "#%d", ID)
#define _sixel_pixel_select(stream, n) fprintf(stream, "%c", '?' + n)
#define _sixel_newline(stream) fprintf(stream, "-")
#define _sixel_end "\x1b\\"

#ifdef __cplusplus
}
#endif

#endif
