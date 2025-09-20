#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "typesetting_str.h"
#include "typesetting_table.h"

const char *divider[] = {_l_pi, _h_dline, _neq_cross_T, _r_pi};
const char *corner[]  = {_lu_rcorner, _ru_rcorner, _ld_rcorner, _rd_rcorner};

int get_height(const char** content){
	int i;
	for(i = 0 ; content[i] != NULL ; i++);
	return i;
}

int* table_division(const char*** content, int len){
	int i, j, col, *ratio, total = 0, temp = 0, remainder = 0;
	for(col = 0 ; content[0] != NULL && content[0][col] != NULL ; col++);
	ratio = malloc(col * sizeof(int));
	if(ratio == NULL) return NULL;
	int longest_vocab[col], show_width[col];
	memset(longest_vocab, 0, col * sizeof(int));
	memset(show_width, 0, col * sizeof(int));
	for(i = 0 ; i < col ; i++){
		for(j = 0 ; content[j] != NULL && content[j][i] != NULL ; j++){
			int tmp = max_vocab(content[j][i]);
			if(tmp > longest_vocab[i]) longest_vocab[i] = tmp;
			tmp = width(content[j][i]);
			if(tmp > show_width[i]) show_width[i] = tmp;
		}
		total += show_width[i];
	}
	if(total == 0){
		free(ratio);
		return NULL;
	}
	len -= col + 1;
	for(i = 0 ; i < col ; i++){
		ratio[i] = len * show_width[i] / total;
		temp += ratio[i];
	}
	remainder = len - temp;
	while(remainder > 0){
		bool flag = 0;
		for(i = 0 ; i < col && remainder > 0 ; i++)
			if(ratio[i] < longest_vocab[i]){
				ratio[i]++;
				remainder--;
				flag = 1;
			}
		if(!flag) break;
	}
	while(remainder > 0)
		for(i = 0 ; i < col && remainder > 0 ; i++){
			ratio[i]++;
			remainder--;
		}
	return ratio;
}

int* average_table_division(int len, int col){
	int i, *temp = malloc(col * sizeof(int));
	if(temp == NULL) return NULL;
	for(i = 0 ; i < col ; i++) temp[i] = len / col;
	return temp;
}

char* table_top(int *arr, int n, int len){
	int i, j;
	char *temp = malloc(strlen(_h_line) * len + 1);
	if(temp == NULL) return NULL;
	temp[0] = 0;
	strcat(temp, corner[0]);

	for(i = 0 ; i < n ; i++){
		for(j = 0 ; j < arr[i] ; j++)
			strcat(temp, _h_line);
		strcat(temp, (i == n - 1) ? corner[1] : _T);
	}
	return temp;
}

char* table_div(int *arr, int n, int len){
	int i, j;
	char *temp = malloc(strlen(divider[0]) * len + 1);
	if(temp == NULL) return NULL;

	temp[0] = 0;
	strcat(temp, divider[0]);

	for(i = 0 ; i < n ; i++){
		for(j = 0 ; j < arr[i] ; j++)
			strcat(temp, divider[1]);
		strcat(temp, divider[(i == n - 1) ? 3 : 2]);
	}
	return temp;
}

char* table_ndiv(int *arr, int n, int len){
	int i, j;
	char *temp = malloc(strlen(_l_T) * len + 1);
	if(temp == NULL) return NULL;
	temp[0] = 0;
	strcat(temp, _l_T);

	for(i = 0 ; i < n ; i++){
		for(j = 0 ; j < arr[i] ; j++)
			strcat(temp, _h_line);
		strcat(temp, (i == n - 1) ? _r_T : _cross_T);
	}
	return temp;
}

char* table_bottom(int *arr, int n, int len){
	int i, j;
	char *temp = malloc(strlen(corner[0]) * len + 1);
	if(temp == NULL) return NULL;
	temp[0] = 0;
	strcat(temp, corner[2]);

	for(i = 0 ; i < n ; i++){
		for(j = 0 ; j < arr[i] ; j++)
			strcat(temp, _h_line);
		strcat(temp, (i == n - 1) ? corner[3] : _rT);
	}
	return temp;
}

char* table_content(char** content, int *arr, int col){
	int i, j, size = (col + 1) * strlen(_v_line) + 2 * col + 1;

	for(i = 0 ; i < col ; i++){
		if(content[i] != NULL)
			size += strlen(content[i]);
		else size += arr[i];
	}
	
	char *temp = malloc(size);
	if(temp == NULL) return NULL;
	temp[0] = 0;
	strcat(temp, _v_line);
	for(i = 0 ; i < col ; i++){
		if(content[i] != NULL)
			strcat(temp, content[i]);
		else
			for(j = 0 ; j < arr[i] ; j++)
				strcat(temp, " ");

		strcat(temp, _v_line);
	}
	return temp;
}

char** table_row_content(char** content, int *ratio, int *alignment, int col){
	int i, j, delta;

	char **result[col], **temp = NULL;
	int height[col], max = 0;

	// Max height
	for(i = 0 ; i < col ; i++){
		result[i] = strcut(content[i], ratio[i], strcut_index);
		height[i] = get_height((const char**)result[i]);
		if(max < height[i]) max = height[i];
	}

	for(i = 0 ; i < col ; i++){
		temp = realloc(result[i], max * sizeof(char*));
		if(temp == NULL)
			for(j = 0 ; j < i ; j++)
				free(result[j]);
		result[i] = temp;
		delta = (max - height[i]) / 2;
		if(delta > 0)
			for(j = height[i] - 1 ; j >= 0 ; j--){
				result[i][j + delta] = result[i][j];
				result[i][j] = NULL;
			}
	}
	char *line[col], **final = malloc((max + 1) * sizeof(char*));
	int len;

	for(i = 0 ; i < max ; i++){
		len = 0;
		for(j = 0 ; j < col ; j++){
			line[j] = align(result[j][i], ratio[j], alignment[j]);
			if(line[j] != NULL)
				len += strlen(line[j]);
		}
		final[i] = table_content(line, ratio, col);
	}
	final[max] = NULL;
	return final;
}

char** table_generate(char*** content, int *alignment, int *ratio){
	int i, j, k = 0, col, row, len, lines = 0;
	for(row = 0 ; content[row] != NULL ; row++);
	for(col = 0 ; content[0][col] != NULL ; col++);
	len = col + 2;
	for(i = 0 ; i < col ; i++) len += ratio[i];
	char **result[row];
	int r[row];
	for(i = 0 ; i < row ; i++){
		result[i] = table_row_content(content[i], ratio, alignment, col);
		r[i] = get_height((const char**)result[i]);
		lines += r[i];
	}
	lines += row + 2;
	char **final = malloc(lines * sizeof(char*));

	for(i = 0 ; i < row ; i++){
		if(i == 0)
			final[k++] = table_top(ratio, col, len);
		else if(i == 1)
			final[k++] = table_div(ratio, col, len);
		else
			final[k++] = table_ndiv(ratio, col, len);
		
		for(j = 0 ; j < r[i] ; j++)
			final[k++] = result[i][j];
	}
	final[k++] = table_bottom(ratio, col, len);
	final[k] = NULL;
	for(i = 0 ; i < row ; i++)
		free(result[i]);
	return final;
}
