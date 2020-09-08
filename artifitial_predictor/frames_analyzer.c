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
	
	/*vector a = { 1, 1, 1 };
	vector b = { 2, 2, 3 };
	vector c = { 2, 3, 3 };

	print_vector(&a);
	print_vector(&b);
	print_vector(&c);
	calculate_water_orientational_distribution(a, b, c, NULL, 0, 0);*/
	//get_matrix_dimensions(&m2, &x, &y);
	//printf("Dimensions: x = %i y = %i\n", x, y);

	read_pdb_file(link_on_path_argument);
	prepare_memory_for_data_storage();
	proceed_pdb_data();
}

