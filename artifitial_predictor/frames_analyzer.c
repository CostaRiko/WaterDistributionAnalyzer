#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "constants.h"
#include "pdb.h"
#include "protein_based_calculations.h"

#define CA 0
#define CB 1
#define N 2

#define DISTANCE 5

int wmain(int argc, wchar_t * argv[]) {
	wprintf(L"CMD LENGTH: %i\n", argc);
	if (argc < 2) {
		wprintf(L"FIRST_ARGUMENT: %ws", argv[0]);
		wprintf(TUTORIAL);
		return 0;
	}
	else if (argc == 3) {
		wprintf(L"COMMAND LINE ARGUMENTS: %ws %ws\n", argv[1], argv[2]);
	}
	wchar_t* link_on_path_argument = argv[1];
	int res = 0;
	
	/*vector a = { 1, 1, 1 };
	vector b = { -2, -2, -3 };
	vector c = { 2, 3, 3 };

	atom test[3] = {
		{0, "OH2\0", "ARG\0", 0, {1, 1, 1} },
		{0, "H1\0", "ARG\0", 0, {2, 2, 2} },
		{0, "H2\0", "ARG\0", 0, {2, 2, 3} }
	};

	calculate_water_orientational_distribution(&a, &b, &c, test, 3, 5);*/
	/*print_vector(&b);
	print_vector(&c);*/
	//get_matrix_dimensions(&m2, &x, &y);
	//printf("Dimensions: x = %i y = %i\n", x, y);

	/*vector a = { 21.399000, 1.691000, 14.135000 };
	vector b = { 21.154000, 1.974000, 13.241000 };
	shift(&a, &b);
	return 0;*/

	read_pdb_file(link_on_path_argument);
	prepare_memory_for_data_storage();
	proceed_pdb_data();

	/*
		Требуется отладка функции prepare_memory_for_data_storage
		Неверно вычисляется размер массива данных о белке
		Возможно неверно вычисляются вообще все размеры массивов
	*/
	HANDLE hep = GetProcessHeap();
	atom* calculation_sequence = (atom*)HeapAlloc(hep, HEAP_ZERO_MEMORY, sizeof(atom) * 3);
	ZeroMemory(calculation_sequence, sizeof(calculation_sequence) * 3);
	calculation_sequence[0].id = -1;
	calculation_sequence[1].id = -1;
	calculation_sequence[2].id = -1;

	int calculation_result = 0;
	
	for (int i = 0; i < protein_size; i++) {
		int test = strcmp("CA ", protein[i].name);
		if ( strcmp("CA ", protein[i].name) != -1 ) {
			calculation_sequence[CA] = protein[i];
		}
		else if ( strcmp("CB ", protein[i].name) != -1 ) {
			calculation_sequence[CB] = protein[i];
		}
		else if ( strcmp("N  ", protein[i].name) != -1 ) {
			calculation_sequence[N] = protein[i];
		}
		if (
			calculation_sequence[CA].id != -1 &&
			calculation_sequence[CB].id != -1 &&
			calculation_sequence[N].id != -1
			) {
			calculation_result = calculate_water_orientational_distribution(
				&calculation_sequence[CA].c,
				&calculation_sequence[CB].c,
				&calculation_sequence[N].c,
				water,
				water_size,
				DISTANCE
			);
			if (calculation_result == -1) {
				wprintf(L"CALCULATION ERROR. PROCEDURE INTERRUPTION");
				break;
			}
			ZeroMemory(calculation_sequence, sizeof(calculation_sequence) * 3);
			calculation_sequence[0].id = -1;
			calculation_sequence[1].id = -1;
			calculation_sequence[2].id = -1;
		}
	}

	free_pdb_memory_stack();
	HeapFree(hep, NULL, calculation_sequence);
}

