#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>
#include "typesetting_str.h"

void typesetting_str_init(){
	setlocale(LC_ALL, "");
}

void delete_new_line(char* content){
	int i, j = 0, len = strlen(content);
	char temp[len + 1];
	for(i = 0 ; i < len ; i++)
		if(content[i] != '\n')
			temp[j++] = content[i];
	temp[j++] = 0;
	memcpy(content, temp, j);
}

void clean(char** result){
	int i;
	if(result == NULL) return;
	for(i = 0 ; result[i] != NULL ; i++)
		free(result[i]);
	free(result);
}

long width(const char* content){
	mbstate_t state = {0};
	const char *ptr = content;
	wchar_t wc;
	long width = 0;
	size_t bytes;

	while((bytes = mbrtowc(&wc, ptr, MB_CUR_MAX, &state)) > 0){
		int w = wcwidth(wc);
		if(w != -1)
			width += w;
		ptr += bytes;
	}
	return width;
}

long max_vocab(const char* content){
	mbstate_t state = {0};
	const char *ptr = content;
	wchar_t wc;
	long max = -1, vocabulary = 0;
	size_t bytes;

	while((bytes = mbrtowc(&wc, ptr, MB_CUR_MAX, &state)) > 0){
		int w = wcwidth(wc);
		if(w < 0) w = 2;
		else if(w == 1 && !iswpunct(w) && !iswspace(w))
			vocabulary++;
		else{
			if(vocabulary > max)
				max = vocabulary;
			vocabulary = 0;
			if(w > max) max = w;
		}
		ptr += bytes;
	}
	
	return (vocabulary > max) ? vocabulary : max;
}

long strcut_index(const char *content, long show_wid){
	mbstate_t state = {0};
	const char *ptr = content;
	wchar_t wc;
	long width = 0;
	size_t i = 0, bytes = 0, w1 = 0, w2 = 0;

	while((bytes = mbrtowc(&wc, ptr, MB_CUR_MAX, &state)) > 0){
		int w = wcwidth(wc);
		if(w < 0) w = 2;
		else if(wc <= 0x7F){
			if(iswspace(wc) || iswpunct(wc))
				w1 = i;
			else if(w2 > w1)
				w1 = i;
		}
		else if(w == 1)
			w1 = i;
		else
			w2 = i;
		width += w;
		ptr += bytes;
		i += bytes;
		if(width > show_wid) break;
	}
	if(width <= show_wid)
		return i;
	if(w1 == w2 && w2 == 0)
		return strict_strcut_index(content, show_wid);
	return (w1 > w2) ? w1 : w2;
}

long strict_strcut_index(const char *content, long show_wid){
	mbstate_t state = {0};
	const char *ptr = content;
	wchar_t wc;
	long width = 0;
	size_t i = 0, bytes = 0;

	while((bytes = mbrtowc(&wc, ptr, MB_CUR_MAX, &state)) > 0){
		int w = wcwidth(wc);
		if(w < 0) w = 2;
		if(w > show_wid) return -1;
		if(width + w > show_wid) break;
		width += w;
		ptr += bytes;
		i += bytes;
	}
	return i;
}

char** strcut(const char* content, long show_wid, long (*function)(const char*, long)){
	char **result = NULL, **temp = NULL;
	const char *ptr = content;
	int base = 10, capacity = 0, current = 0;
	long i = 0, j = 0;
	
	while(*ptr){
		if(current >= capacity){
			capacity += base;
			temp = realloc(result, capacity * sizeof(char*));
			if(temp == NULL){
				if(result != NULL){
					for(j = 0 ; j < current ; j++)
						free(result[j]);
					free(result);
				}
				return NULL;
			}
			else result = temp;
		}
		i = function(ptr, show_wid);
		if(i == -1){
			if(result != NULL){
				for(j = 0 ; j < current ; j++)
					free(result[j]);
				free(result);
			}
			return NULL;
		}
		if(i == 0) break;
		result[current] = malloc(i + 1);
		if(result[current] == NULL){
			for(j = 0 ; j < current ; j++)
				free(result[j]);
			free(result);
			return NULL;
		}
		memcpy(result[current], ptr, i);
		result[current][i] = 0;
		current++;
		ptr += i;
	}
	temp = realloc(result, (current + 1) * sizeof(char*));
	if(temp == NULL){
		for(j = 0 ; j < current ; j++)
			free(result[j]);
		free(result);
		return NULL;
	}
	result = temp;
	result[current++] = NULL;
	return temp;
}

/** align type **/
/* left   align = -1 */
/* centre align = 0  */
/* right  align = 1  */
char* align(const char *content, int show_wid, int type){
	if(content == NULL) return NULL;
	int available = show_wid - width(content);
	int size = strlen(content);
	int left = 0, right = 0;

	if(type == 1)
		left = available;
	else if(type == 0){
		left = available / 2;
		right = available - left;
	}
	else
		right = available;
	
	if(available < 0) return NULL;

	char *result = NULL;
	result = malloc(size + available + 1);
	if(result == NULL) return NULL;
	result[0] = 0;
	if(left != 0){
		memset(result, ' ', left);
		result[left] = 0;
	}
	memcpy(result + left, content, strlen(content));
	if(right != 0)
		memset(result + left + size, ' ', right);
	result[size + available] = 0;
	return result;
}

char* indentation(const char *content, int indent, const char* indent_content){
	char *result = NULL;
	int i, len = strlen(indent_content), sum = strlen(content) + indent * len + 1;
	result = malloc(sum);
	if(result == NULL) return NULL;
	memset(result, 0, sum);
	for(i = 0 ; i < indent * len ; i += len)
		memcpy(&result[i], indent_content, len);
	memcpy(&result[i], content, strlen(content));
	return result;
}

char* unfold_tabs(const char *content, int spaces){
	int i, j = 0, k, len = strlen(content), tabs = 0;

	for(i = 0 ; i < len ; i++)
		if(content[i] == '\t')
			tabs++;
	
	char *result = malloc(len + tabs * spaces + 1);

	if(result == NULL) return NULL;
	
	for(i = 0 ; i < len ; i++){
		if(content[i] == '\t')
			for(k = 0 ; k < spaces ; k++)
				result[j++] = ' ';
		else
			result[j++] = content[i];
	}
	result[j] = 0;
	return result;
}
