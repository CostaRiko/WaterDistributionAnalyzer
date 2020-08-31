#include <Windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "constants.h"
#include "pdb.h"
#include "protein_based_calculations.h"

int wmain(int argc, wchar_t * argv[]) {
	printf(L"CMD LENGTH: %i\n", argc);
	if (argc < 2) {
		wprintf(L"FIRST_ARGUMENT: %ws", argv[0]);
		wprintf(TUTORIAL);
		return 0;
	}
	else if (argc == 3) {
		wprintf(L"COMMAND LINE ARGUMENTS: %ws %ws\n", argv[1], argv[2]);
	}
	wchar_t* link_on_path_argument = argv[1];
	double m1[3][3] = {
		{1, 2, 3},
		{2, 3, 4},
		{3, 4, 5}
	}; 
	
	double m2[3][3] = {
		{4, 5, 6},
		{5, 6, 7},
		{6, 7, 8}
	};

	double a[3] = { 1, 1, 1 };
	double b[3] = { 2, 2, 2 };
	printf("DISTANCE: %lf\n", distance(a, b));
	//get_matrix_dimensions(&m2, &x, &y);
	//printf("Dimensions: x = %i y = %i\n", x, y);

	//read_pdb_file(link_on_path_argument);
	//prepare_memory_for_data_storage();
	//proceed_pdb_data();
}

