#ifndef ESCAPE_CHARACTER_CLEAN_H
#define ESCAPE_CHARACTER_CLEAN_H

#ifdef _cplusplus
extern "C"{
#endif

#define _clear "\x1b[2J\x1b[1;1H"
#define _clear_line_end "\x1b[0K"
#define _clear_line_start "\x1b[1K"
#define _clear_line "\x1b[2K"

#ifdef _cplusplus
}
#endif

#endif
