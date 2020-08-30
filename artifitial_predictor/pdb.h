#pragma once
#include <Windows.h>

BOOL read_pdb_file(wchar_t * path);
BOOL proceed_pdb_data();
BOOL proceed_pdb_line(char* line);