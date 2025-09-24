#ifndef _FILES_DEALER_H
#define _FILES_DEALER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

extern const char *home;

bool getHome();
char* getName(const char*);
const char* getType(const char*);
bool isBlkDev(const char*);
bool isCharDev(const char*);
bool isFIFO(const char*);
bool isSocket(const char*);
bool isRead(const char*);
bool isWrite(const char*);
bool isLink(const char*);
bool isDir(const char*);
bool isFile(const char*);
bool isExec(const char*);
char** dir_content(const char*);
#endif
