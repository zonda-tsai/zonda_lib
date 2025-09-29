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

size_t zonda_min(size_t a, size_t b){
	return (a < b) ? a : b;
}

size_t zonda_max(size_t a, size_t b){
	return (a > b) ? a : b;
}
