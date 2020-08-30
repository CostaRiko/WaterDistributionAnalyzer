#include <Windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "constants.h"
#include "pdb.h"

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
	read_pdb_file(link_on_path_argument);
	proceed_pdb_data();
}

