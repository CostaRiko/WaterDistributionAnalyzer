#pragma once
#include <Windows.h>

BOOL read_pdb_file(wchar_t * path);
BOOL proceed_pdb_data();
BOOL proceed_pdb_line(char* line, const int linelen);
BOOL str_is(char* str1, const char* str2);
int substring(char* str, int start, int finish, int linelen, char* result, int result_len);