#ifndef TYPESETTING_STRING_PART
#define TYPESETTING_STRING_PART

void typesetting_str_init();
void delete_new_line(char*);
long width(const char*);
long max_vocab(const char*);
long strcut_index(const char*, long);
long strict_strcut_index(const char*, long);
char** strcut(const char*, long, long(*function)(const char*, long));
char* align(const char*, int, int);
char* indentation(const char*, int, const char*);
char* unfold_tabs(const char*, int);

#endif
