#ifndef TYPESETTING_CONTAINER_TABLE_H
#define TYPESETTING_CONTAINER_TABLE_H

#include <stdio.h>
#include <string.h>
#include <../glyph/glyph_lib.h>

extern const char *divider[];

// round corner
extern const char *corner[];

// sharp corner
// const char *corner[] = {_lu_corner, _ru_corner, _ld_corner, _rd_corner};

typedef struct{
	long row, col;
	char ***content;
	int *alignment;
}table;

int* table_division(const char***, int);
int* average_table_division(int, int);
char* table_top(int*, int, int);
char* table_div(int*, int, int);
char* table_ndiv(int*, int, int);
char* table_bottom(int*, int, int);
char* table_content(char**, int*, int);
char** table_row_content(char**, int*, int*, int);
char** table_generate(char***, int*, int*);

#endif
