#include "pdb.h"

char* reading_buffer; // Память не возвращена системе!!!
DWORD file_size;

BOOL read_pdb_file(wchar_t* path)
{
	wprintf(L"PDB %s READING ...\n", path);
	HANDLE file = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE) {
		wprintf(L"PDB OPENING ERROR");
		return FALSE;
	}
	DWORD readed_bytes = 1;
	file_size = GetFileSize(file, &file_size); file_size++;
	reading_buffer = malloc(file_size);
	ZeroMemory(reading_buffer, file_size);
	BOOL reading_statement = ReadFile(file, reading_buffer, file_size, &readed_bytes, NULL);
	if (!reading_statement) {
		wprintf(L"PDB READING ERROR\n");
		return FALSE;
	}
	return TRUE;
}
BOOL proceed_pdb_data()
{
	char line[128];
	int line_cursor = 0;
	ZeroMemory(line, 128);
	for (int i = 0; i < file_size; i++) {
		if (reading_buffer[i] == '\n') {
			proceed_pdb_line(line);
			ZeroMemory(line, 128);
			line_cursor = 0;
		}
		else {
			line[line_cursor] = reading_buffer[i];
			line_cursor++;
		}
	}
	free(reading_buffer);
	return TRUE;
}

BOOL proceed_pdb_line(char* line)
{

	return TRUE;
}




