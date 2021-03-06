#include "pdb.h"

char* reading_buffer = NULL;
DWORD file_size;
atoms_count = 0, atoms_cursor = 0;
water_size = 0, water_cursor = 0;
protein_size = 0, protein_cursor = 0;
BOOL ter_is_achieved = FALSE;
HANDLE hHeap, hprot, hwat;

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
	hHeap = GetProcessHeap();
	reading_buffer = (char*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, file_size);
	BOOL reading_statement = ReadFile(file, reading_buffer, file_size, &readed_bytes, NULL);
	if (!reading_statement) {
		wprintf(L"PDB READING ERROR\n");
		return FALSE;
	}
	printf("file_size: %i\n", file_size);
	return TRUE;
}

char line[128];

void proceed_pdb_data()
{
	ter_is_achieved = FALSE;
	int line_cursor = 0;
	ZeroMemory(line, 128);
	double percents = 0;
	for (unsigned int i = 0; i < file_size; i++) {
		percents = (double)i * 100/ file_size;
		//printf("\tproceed_pdb_data: %lf\r", percents);
		if (reading_buffer[i] == '\n') {
			proceed_pdb_line(line, 128);
			ZeroMemory(line, 128);
			line_cursor = 0;
		}
		else {
			line[line_cursor] = reading_buffer[i];
			line_cursor++;
		}
	}
	//printf("\r                                                                              \r");
	ZeroMemory(reading_buffer, file_size);
	HeapFree(hHeap, NULL, reading_buffer);
}

BOOL proceed_pdb_line(char* line, const int linelen)
{
	int 
		id, resSeq, substring_status; 

	double 
		x, y, z; 

	char 
		idc[6], name[4], res[6], record[6], resSeqc[4], xc[8], yc[8], zc[8];

	ZeroMemory(idc, 6); ZeroMemory(name, 4); ZeroMemory(res, 6); ZeroMemory(record, 6);
	ZeroMemory(resSeqc, 4); ZeroMemory(xc, 4); ZeroMemory(yc, 5);  ZeroMemory(zc, 6);

	if (strcmp(line, "END") != -1) {
		ter_is_achieved = TRUE;
		return TRUE;
	}

	substring_status = substring(line, 7,  12, linelen, idc, 6);
	substring_status = substring(line, 0, 7, linelen, record, 6);
	substring_status = substring(line, 13, 17, linelen, name, 4);
	substring_status = substring(line, 17, 21, linelen, res,6);
	substring_status = substring(line, 23, 27, linelen, resSeqc, 4);
	substring_status = substring(line, 31, 38, linelen, xc, 8);
	substring_status = substring(line, 38, 46, linelen, yc, 8);
	substring_status = substring(line, 46, 55, linelen, zc, 8);

	if ( ! str_is(record, 6, "ATOM  ", 6)) {
		return FALSE;
	}
	id = atoi(idc);
	resSeq = atoi(resSeqc);
	x = atof(xc);
	y = atof(yc);
	z = atof(zc);
	name[3] = '\0';
	res[5] = '\0';
	if ( strcmp(res, "TIP3") != -1 ) {
		water[water_cursor].id = id;
		strcpy( water[water_cursor].name,  name);
		strcpy( water[water_cursor].res, res);
		water[water_cursor].resSeq = resSeq;
		water[water_cursor].c.x = x;
		water[water_cursor].c.y = y;
		water[water_cursor].c.z = z;
		water_cursor++;
	}
	else {
		if (!ter_is_achieved) {
			protein[protein_cursor].id = id;
			strcpy(protein[protein_cursor].name, name);
			strcpy(protein[protein_cursor].res, res);
			protein[protein_cursor].resSeq = resSeq;
			protein[protein_cursor].c.x = x;
			protein[protein_cursor].c.y = y;
			protein[protein_cursor].c.z = z;
			protein_cursor++;
		}
	}
	return TRUE;
}

BOOL str_is(char* str1, int str1len, const char* str2, int str2len)
{
	if (str1len == str2len) {
		for (int i = 0; i < str1len; i++) {
			if (str1[i] != str2[i]) {
				return FALSE;
			}
		}
	}
	else {
		return FALSE;
	}
	return TRUE;
}

BOOL prepare_memory_for_data_storage(void)
{
	char line[128];
	int line_cursor = 0;
	ZeroMemory(line, 128);
	ter_is_achieved = FALSE;
	double percents = 0;
	for (unsigned int i = 0; i < file_size; i++) {
		percents = (double)i * 100 / file_size;
		//printf("\tprepare_memory_for_data_storage: %lf\r", percents);
		if (reading_buffer[i] == '\n') {
			if ( strstr(line, "ATOM  ") != 0 ) {
				atoms_count++;
				if ( strstr(line, "TIP3") != 0 ) {
					water_size++;
				}
				else {
					if ( ! ter_is_achieved ) {
						protein_size++;
					}
				}
			}
			if (strstr(line, "END") != 0 ) {
				ter_is_achieved = TRUE;
			}
			ZeroMemory(line, 128);
			line_cursor = 0;
		}
		else {
			line[line_cursor] = reading_buffer[i];
			line_cursor++;
		}
	}
	//printf("\r                                                                              \r");
	hprot = GetProcessHeap();
	hwat = GetProcessHeap();
	protein = (atom*)HeapAlloc(hprot, HEAP_ZERO_MEMORY, sizeof(atom)*protein_size);
	water = (atom*)HeapAlloc(hwat, HEAP_ZERO_MEMORY, sizeof(atom)*water_size);

	return TRUE;
}

int substring(char* str, int start, int finish, int linelen, char * result, int result_len)
{
	int result_cursor = 0;
	for (int i = start; i < finish; i++) {
		if (result_cursor >= result_len) {
			return 1;
		}
		result[result_cursor] = str[i];
		result_cursor++;
	}
	return 0;
}

void free_pdb_memory_stack()
{
	HeapFree(hwat, NULL, water);
	HeapFree(hprot, NULL, protein);
}




