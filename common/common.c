#include <stdio.h>
#include <stdlib.h>
#include "common.h"

void clean(char** result){
	if(result == NULL) return;
	int i;
	for(i = 0 ; result[i] != NULL ; i++)
		free(result[i]);
	free(result);
}

size_t min(size_t a, size_t b){
	return (a < b) ? a : b;
}

size_t max(size_t a, size_t b){
	return (a > b) ? a : b;
}
