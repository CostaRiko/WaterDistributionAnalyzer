#ifndef PDB
#define PDB 1
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "protein_based_calculations.h"

#pragma warning( disable : 4996 )

typedef struct ATOM {
	int id;
	char name[4];
	char res[6];
	int resSeq;
	vector c;
} atom;

atom* protein; // ������ �� ���������� �������
atom* water;   // ������ �� ���������� �������

BOOL read_pdb_file(wchar_t * path);
BOOL proceed_pdb_data(void);
BOOL proceed_pdb_line(char* line, const int linelen);
BOOL str_is(char* str1, const char* str2);
BOOL prepare_memory_for_data_storage(void);
int substring(char* str, int start, int finish, int linelen, char* result, int result_len);


#endif
/*
1) ��������� ���������� ������ �����										+
2) ��������� ���������� ������ ����											+
3) �������� ������ ��� ����� � ����											+
4) �������� ������ ��� ����� ���������� ����� � ����		( �������� ������������ ���������� )
5) ��������� ����� ���������� ����� � ����
6) ������� ����� �� ����� �����
7) ��� ������� ����� ����� �������� ������ ��� ������ ������� ����
8) ��������� ���������� ��� ����������� ������
9) �������� �������� png
*/