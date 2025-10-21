#ifndef _SCREEN_BUFFER_BRANCH
#define _SCREEN_BUFFER_BRANCH

#ifdef __cplusplus
extern "C"{
#endif

#define _enable_screen_buffer "\x1b[?1049h"
#define _disable_screen_buffer "\x1b[?1049l"

#ifdef __cplusplus
}
#endif

#endif
