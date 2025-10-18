#ifndef MOUSE_CONTROL_BRANCH
#define MOUSE_CONTROL_BRANCH

#ifdef __cplusplus
extern "C" {
#endif

#include "win_term_mode.h"

#define _enable_mouse_sgr "\x1b[?1006h"
#define _disable_mouse_sgr "\x1b[?1006l"
#define _enable_mouse_drag "\x1b[?1002h"
#define _disable_mouse_drag "\x1b[?1002l"
#define _enable_mouse_all "\x1b[?1002h"
#define _disable_mouse_all "\x1b[?1003l"

#ifdef __cplusplus
}
#endif

#endif
