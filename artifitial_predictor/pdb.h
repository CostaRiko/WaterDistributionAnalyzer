#ifndef PDB
#define PDB 1
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "protein_based_calculations.h"

#pragma warning( disable : 4996 )

typedef struct {
	double x;
	double y;
	double z;
} vector;

typedef struct {
	int id;
	char name[4];
	char res[6];
	int resSeq;
	vector c;
} atom;

atom* protein; // Память не возвращена системе
atom* water;   // Память не возвращена системе

BOOL read_pdb_file(wchar_t * path);
BOOL proceed_pdb_data(void);
BOOL proceed_pdb_line(char* line, const int linelen);
BOOL str_is(char* str1, int str1len, const char* str2, int str2len);
BOOL prepare_memory_for_data_storage(void);
int substring(char* str, int start, int finish, int linelen, char* result, int result_len);
void rotation_x(vector* v, double cosa, double sina);
void rotation_y(vector* v, double cosa, double sina);
void rotation_z(vector* v, double cosa, double sina);
void shift(vector* shift, vector* vector);
double distance(vector* a, vector* b);
double check_if_null(double val);
double cos_a(vector* a, vector* b);
/*
	return
*/
int calculate_water_orientational_distribution(vector* CA, vector* CB, vector* N, atom* HOHa, int HOHa_len, double dist);
double sin_from_cos(double cosa);
void print_vector(vector* a, const char* description);


#endif
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