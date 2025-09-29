#ifndef WCWIDTH_H
#define WCWIDTH_H

#include <wchar.h>

int wcwidth(wchar_t ucs);
int wcswidth(const wchar_t *pwcs, size_t n);

#endif
