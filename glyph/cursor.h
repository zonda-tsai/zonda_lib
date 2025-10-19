#ifndef CURSORS_H
#define CURSORS_H

#ifdef _cplusplus
extern "C"{
#endif
	
#define _cursor_to(stream, x, y) fprintf(stream, "\x1b[%d;%dH", y, x)
#define _cursor_u(stream, n) fprintf(stream, "\x1b[%dA", n)
#define _cursor_d(stream, n) fprintf(stream, "\x1b[%dB", n)
#define _cursor_f(stream, n) fprintf(stream, "\x1b[%dC", n)
#define _cursor_b(stream, n) fprintf(stream, "\x1b[%dD", n)
#define _cursor_save "\x1b[s"
#define _cursor_restore "\x1b[u"
#define _cursor_hide "\x1b[?25l"
#define _cursor_show "\x1b[?25h"

#ifdef __cplusplus
}
#endif

#endif
