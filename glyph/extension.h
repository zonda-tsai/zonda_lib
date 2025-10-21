#ifndef EXTENSION_BRANCH
#define EXTENSION_BRANCH

#ifdef __cplusplus
extern "C"{
#endif

#define _terminal_title(stream, string) fprintf(stream, "\x1b]0;%s\x07", string);
#define _hyperlink(stream, string, url) fprintf(stream, "\x1b]8;;%s\x1b\\%s\x1b]8;;\x1b\\", url, string);


#ifdef __cplusplus
}
#endif

#endif
