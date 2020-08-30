#pragma once
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

char* reading_buffer;
DWORD file_size;
int atoms_count = 0, atoms_cursor = 0;
int water_size = 0, water_cursor = 0;
int protein_size = 0, protein_cursor = 0;
BOOL ter_is_achieved = FALSE;

typedef struct ATOM {
	int id;
	char name[4];
	char res[6];
	int resSeq;
	double x;
	double y;
	double z;
} atom;

atom* protein; // Память не возвращена системе
atom* water;   // Память не возвращена системе

BOOL read_pdb_file(wchar_t * path);
BOOL proceed_pdb_data(void);
BOOL proceed_pdb_line(char* line, const int linelen);
BOOL str_is(char* str1, const char* str2);
BOOL prepare_memory_for_data_storage(void);
int substring(char* str, int start, int finish, int linelen, char* result, int result_len);

/*
1) Посчитать количество атомов белка										+
2) Посчитать количество атомов воды											+
3) Выделить память под белок и воду											+
4) Выделить память под новые координаты белка и воды		( Обновить существующие координаты )
5) Вычислить новые координаты белка и воды
6) Разбить белок на альфа атомы
7) Для каждого альфа атома выделить память под список молекул воды
8) Вычислить статистику для полученного списка
9) Записать картинку png
*/