#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include "file_system.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

const char *home = NULL;

int cmp(const void *t1, const void *t2){
	const char *a = *(const char**)t1;
	const char *b = *(const char**)t2;
	int result = strcmp(getType(a), getType(b));
	if(result) return result;
	int lena = getType(a) - a, lenb = getType(b) - b;
	int size = min(lena, lenb) - 1;
	result = strncmp(a, b, size);
	if(result == 0){
		if(lena > lenb) return 1;
		else if(lena < lenb) return -1;
		else return 0;
	}
	return result;
}

void sort(char **arr, size_t n){
	qsort(arr, n, sizeof(char*), cmp);
}

bool getHome(){
#ifdef _WIN32
	home = getenv("USERPROFILE");
#else
	home = getenv("HOME");
#endif
	return !(home == NULL);
}

char* getName(const char* file_name){
	if(file_name == NULL) return 0;
	int i, j = 0, len = strlen(file_name);
	if(len <= 0) return NULL;
	for(i = len - 1 ; i >= 0 && file_name[i] != '.' && file_name[i] != '\\' && file_name[i] != '/' ; i--);
	if(i == -1 || file_name[i] == '\\' || file_name[i] == '/'){
		j = i + 1;
		i = len;
	}
	else if(file_name[i] == '.'){
		for(j = i - 1 ; j >= 0 && file_name[j] != '/' && file_name[j] != '\\' ; j--);
		j++;
	}
	if(i - j <= 0) return NULL;
	char *temp = (char*)malloc(i - j + 1);
	if(temp == NULL)
		return NULL;
	strncpy(temp, file_name + j, i - j);
	temp[i - j] = 0;
	return temp;
}

const char* getType(const char* file_name){
	if(file_name == NULL) return "";
	int i;
	for(i = strlen(file_name) - 1 ; i >= 0 && file_name[i] != '.' && file_name[i] != '\\' && file_name[i] != '/' ; i--);
	if(i == -1 || i == 0 || i == (int)strlen(file_name) - 1 || file_name[i] == '\\' || file_name[i] == '/')
		return "";
	return &file_name[i + 1];
}

#ifdef _WIN32

bool isBlkDev(const char* file_name){
	return 0;
}

bool isCharDev(const char* file_name){
	return 0;
}

bool isFIFO(const char* file_name){
	return 0;
}

bool isSocket(const char* file_name){
	return 0;
}

bool isDir(const char* file_name){
	DWORD attrs = GetFileAttributesA(file_name);
	return (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY));
}

bool isFile(const char* file_name){
	DWORD attrs = GetFileAttributesA(file_name);
	return (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY));
}

bool isLink(const char* file_name){
	DWORD attrs = GetFileAttributesA(file_name);
	return (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_REPARSE_POINT));
}

bool isExec(const char* file_name){
	if(!isFile(file_name)) return false;
	const char* ext = getType(file_name);
	return (ext && (_stricmp(ext, "exe") == 0 || _stricmp(ext, "bat") == 0 || _stricmp(ext, "cmd") == 0));
}

bool isRead(const char* file_name){
	DWORD attrs = GetFileAttributesA(file_name);
	if(attrs == INVALID_FILE_ATTRIBUTES) return 0;
	return true;
}

bool isWrite(const char* file_name){
	DWORD attrs = GetFileAttributesA(file_name);
	if(attrs == INVALID_FILE_ATTRIBUTES) return false;
	return !(attrs & FILE_ATTRIBUTE_READONLY);
}

char** dir_content(const char* dirloc){
	WIN32_FIND_DATAW findFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	wchar_t searchPath[PATH_MAX];
	char fullPath[PATH_MAX];
	int n = 0, capacity = 10;
	char** a = NULL;

	MultiByteToWideChar(CP_UTF8, 0, dirloc, -1, searchPath, PATH_MAX);

	wcscat_s(searchPath, PATH_MAX, L"\\*");

	hFind = FindFirstFileW(searchPath, &findFileData);

	if(hFind == INVALID_HANDLE_VALUE)
		return NULL;

	a = malloc(capacity * sizeof(char*));
	if (a == NULL) {
		FindClose(hFind);
		return NULL;
	}

	do{
		if(wcscmp(findFileData.cFileName, L".") == 0 || wcscmp(findFileData.cFileName, L"..") == 0){
			continue;
		}

		if(n >= capacity){
			capacity *= 2;
			char** temp = realloc(a, capacity * sizeof(char*));
			if(temp == NULL){
				clean(a);
				FindClose(hFind);
				return NULL;
			}
			a = temp;
		}

		char fileNameUtf8[PATH_MAX];
		WideCharToMultiByte(CP_UTF8, 0, findFileData.cFileName, -1, fileNameUtf8, PATH_MAX, NULL, NULL);

		snprintf(fullPath, PATH_MAX, "%s\\%s", dirloc, fileNameUtf8);

		a[n] = _strdup(fullPath);
		if(a[n] == NULL){
			clean(a);
			FindClose(hFind);
			return NULL;
		}
		n++;
	}while(FindNextFileW(hFind, &findFileData) != 0);

	FindClose(hFind);

	if(n == 0) {
		free(a);
		return NULL;
	}
	
	a[n] = NULL;
	char** temp = realloc(a, (n + 1) * sizeof(char*));
	if(temp == NULL) {
		clean(a);
		return NULL;
	}
	a = temp;

	sort(a, n); // 如果需要排序，取消註解此行
	return a;
}

#else

bool isBlkDev(const char* file_name){
	struct stat st;
	return (lstat(file_name, &st) != -1 && S_ISBLK(st.st_mode));
}

bool isCharDev(const char* file_name){
	struct stat st;
	return (lstat(file_name, &st) != -1 && S_ISCHR(st.st_mode));
}

bool isFIFO(const char* file_name){
	struct stat st;
	return (lstat(file_name, &st) != -1 && S_ISFIFO(st.st_mode));
}

bool isSocket(const char* file_name){
	struct stat st;
	return (lstat(file_name, &st) != -1 && S_ISSOCK(st.st_mode));
}

bool isLink(const char* file_name){
	struct stat st;
	return ((lstat(file_name, &st) != -1) && S_ISLNK(st.st_mode));
}

bool isDir(const char* file_name){
	struct stat st;
	return (!stat(file_name, &st) && S_ISDIR(st.st_mode));
}

bool isFile(const char* file_name){
	struct stat st;
	return (!stat(file_name, &st) && S_ISREG(st.st_mode));
}

bool isExec(const char* file_name){
	return isFile(file_name) && !access(file_name, X_OK);
}

bool isRead(const char* file_name){
	return isFile(file_name) && !access(file_name, R_OK);
}

bool isWrite(const char* file_name){
	return isFile(file_name) && !access(file_name, W_OK);
}

char** dir_content(const char* dirloc){
	DIR *dir = opendir(dirloc);
	if(dir == NULL) return NULL;
	
	struct dirent *entry;
	struct stat st;
	int n = 0, capacity = 10;
	char name[PATH_MAX], **a = NULL;

	// vector initialization
	a = malloc(capacity * sizeof(char*));
	if(a == NULL){
		closedir(dir);
		return NULL;
	}
	
	while((entry = readdir(dir)) != NULL){
		if(!strcmp(entry->d_name, "..") || !strcmp(entry->d_name, "."))
			continue;
		
		snprintf(name, PATH_MAX, "%s/%s", dirloc, entry->d_name);
		if(lstat(name, &st) == -1)
			continue;

		// vector automatically added
		if(n >= capacity){
			capacity *= 2;
			char **temp = realloc(a, capacity * sizeof(char*));
			if(temp == NULL){
				clean(a);
				closedir(dir);
				return NULL;
			}
			a = temp;
		}

		a[n] = strdup(name);
		if(a[n] == NULL){
			closedir(dir);
			clean(a);
			a = NULL;
			return NULL;
		}
		n++;
	}
	closedir(dir);
	if(n == 0){
		free(a);
		return NULL;
	}
	a[n] = NULL;
	char **temp = realloc(a, (n + 1) * sizeof(char*));
	if(temp == NULL){
		clean(a);
		return NULL;
	}
	a = temp;
	sort(a, n);
	return a;
}

#endif
