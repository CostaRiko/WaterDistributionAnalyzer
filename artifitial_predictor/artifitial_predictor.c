#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>

#pragma warning( disable : 4996 )

#define X 0
#define Y 1
#define Z 2

#define ID 0
#define ATOM_TYPE 1
#define AA 2
#define CA_COORDS 3
#define CB_NAME 4
#define CB_COORDS 5
#define C_NAME 6
#define C_COORDS 7
#define WATER 8

#define CA_OW_DISTANCE 8
#define ON_RADICAL_STAT 1
#define ON_CA_STAT -1

double x_direction_vector[3] = { 1, 0, 0 };
double y_direction_vector[3] = { 0, 1, 0 };
double z_direction_vector[3] = { 0, 0, 1 };

double shift[3] = { 0 };

double past_distance = 0;

int DISTANCE_ERROR = 0;

struct ca_line {
	char aaname[3];
	double 	CA[3];
	double 	CB[3];
	double 	C[3];
	double HOHs[900];
} line;

/*
	Текущие задачи:
	1) Преобразовать функцию combine для работы со структурами atom
	2) Преобразовать функцию combine для работы со структурами atom
*/
void show_point_coords(double* a, char* comment)
{
	printf("Coordinates of %s\t: ( %lf, %lf, %lf )\n", comment, a[0], a[1], a[2]);
}

double determinant(double* a) // a = [ a11, a12, a21, a22 ]
{

	double res = a[0] * a[3] - a[2] * a[1];
	return res;
}

void plane_coefs(double* a, double* b, double* c, double* result)
{
	double d1[] = { b[Y] - a[Y], b[Z] - a[Z], c[Y] - a[Y], c[Z] - a[Z] };
	double d2[] = { b[0] - a[0], b[2] - a[2], c[0] - a[0], c[2] - a[2] };
	double d3[] = { b[0] - a[0], b[1] - a[1], c[0] - a[0], c[1] - a[1] };

	double A = determinant(d1);
	double B = -1 * determinant(d2);
	double C = determinant(d3);
	double D = -1 * a[0] * A + a[1] * (-1) * B - a[2] * C;
	result[0] = A; result[1] = B; result[2] = C; result[3] = D;
}

double cos_a(double* plane_coefs, double* direction_vector)
{
	double res = (plane_coefs[0] * direction_vector[0] + plane_coefs[1] * direction_vector[1] + plane_coefs[2] * direction_vector[2]) / (sqrt(pow(plane_coefs[0], 2) + pow(plane_coefs[1], 2) + pow(plane_coefs[2], 2)) * sqrt(pow(direction_vector[0], 2) + pow(direction_vector[1], 2) + pow(direction_vector[2], 2)));
	return res;
}

void rotation_x(double* p, double sina, double cosa, double* res)
{
	/*x*/ res[0] = p[0];
	/*y*/ res[1] = p[1] * cosa - p[2] * sina;
	/*z*/ res[2] = p[1] * sina + p[2] * cosa;
}

void rotation_y(double* p, double sina, double cosa, double* res)
{
	/*x*/ res[0] = p[0] * cosa + p[2] * sina;
	/*y*/ res[1] = p[1];
	/*z*/ res[2] = (-1) * p[0] * sina + p[2] * cosa;
}

void rotation_z(double* p, double sina, double cosa, double* res)
{
	/*x*/ res[0] = p[0] * cosa - p[1] * sina;
	/*y*/ res[1] = p[0] * sina + p[1] * cosa;
	/*z*/ res[2] = p[2];
}

void combine(double* CA, double* CB, double* C)
{
	shift[X] = CA[X]; shift[Y] = CA[Y]; shift[Z] = CA[Z];
	CB[0] = CB[0] - CA[0]; CB[1] = CB[1] - CA[1]; CB[2] = CB[2] - CA[2];
	C[0] = C[0] - CA[0]; C[1] = C[1] - CA[1]; C[2] = C[2] - CA[2];
	CA[0] = 0; CA[1] = 0; CA[2] = 0;
}

void print_vector(double* v, const char* comment)
{
	printf("%s: %lf, %lf, %lf\n", comment, v[0], v[1], v[2]);
}

int check_distance(double d)
{
	if (past_distance == d) {
		printf("past_distance = %lf d = %lf check = %i\n", past_distance, d, past_distance != d);
		printf("ИЗМЕНЕНИЕ РАССТОЯНИЯ! ОШИБКА РАССЧЕТОВ! ОСТАНОВКА ВЫЧИСЛЕНИЯ\n");
		DISTANCE_ERROR = 1;
		return 0;
	}
	return 1;
}

void update_three_points_coords(double* CA, double* CB, double* C, double* sin_seq, double* cos_seq)
{
	combine(CA, CB, C);
	//printf("Сдвиг начала координат в точку CA\n");
	double distance = sqrt(pow(CB[0] - C[0], 2) + pow(CB[1] - C[1], 2) + pow(CB[2] - C[2], 2));
	past_distance = distance;
	//printf("Расстояние C - СB %lf\n", distance);

	double XOY_projection[3] = { C[0] - CA[0], C[1] - CA[1], 0 };

	//print_vector(direction_vector, "Вектор направления");
	//print_vector(XOY_projection, "Проекция вектора направления");

	//Получаем значения синуса и косинуса угла фи икс
	double cosa = cos_a(XOY_projection, x_direction_vector);
	double sina = sqrt(1 - pow(cosa, 2));

	// Если Y > 0, необходимо вращение противоположное ориентации плоскости

	if (XOY_projection[Y] > 0) {
		sina = -sina;
	}

	//printf("1) cosa: %lf sina: %lf\n", cosa, sina);

	sin_seq[0] = sina;
	cos_seq[0] = cosa;

	double C1[3] = { 0 };
	rotation_z(C, sina, cosa, C1);
	double CB1[3] = { 0 };
	rotation_z(CB, sina, cosa, CB1);
	//print_vector(C1, "Точка С после поворота на фи икс");

	// Находим угол между проекцией CA - C и прямой CA - C

	cosa = cos_a(C1, x_direction_vector);
	sina = sqrt(1 - pow(cosa, 2));

	// Если Z > 0, необходимо вращение противоположное ориентации плоскости

	if (C1[Z] > 0) {
		sina = -sina;
	}

	//printf("2) cosa: %lf sina: %lf\n", cosa, sina);

	sin_seq[1] = sina;
	cos_seq[1] = cosa;

	double C2[3] = { 0 };
	rotation_y(C1, -sina, cosa, C2);
	double CB2[3] = { 0 };
	rotation_y(CB1, -sina, cosa, CB2);
	//print_vector(C2, "Точка С после поворота на пси икс");

	// Ось Х совмещена с прямой СА - С. Требуется совмещение плоскости ХОУ с точкой CB вращением вокруг X

	double YOZ_CB_projection[3] = { 0, CB2[1], CB2[2] };
	//print_vector(YOZ_CB_projection, "XOYCB");
	cosa = cos_a(YOZ_CB_projection, y_direction_vector);
	sina = sqrt(1 - pow(cosa, 2));

	if (CB2[Z] > 0) {
		sina = -sina;
	}

	sin_seq[2] = sina;
	cos_seq[2] = cosa;

	//printf("3) cosa: %lf sina: %lf\n", cosa, sina);

	double CB3[3] = { 0 };
	rotation_x(CB2, sina, cosa, CB3);
	//print_vector(CB3, "Точка СB после серии поворотов");
	distance = sqrt(pow(CB3[0] - C2[0], 2) + pow(CB3[1] - C2[1], 2) + pow(CB3[2] - C2[2], 2));
	//printf("Расстояние C - СB %lf\n", distance);

	if (fabs(past_distance - distance) > 0.000000001) {
		printf("ИЗМЕНЕНИЕ РАССТОЯНИЯ! ОШИБКА РАССЧЕТОВ! РЕЗУЛЬТАТЫ НЕКОРРЕКТНЫ!\n");
	}

	CB[0] = CB3[0]; CB[1] = CB3[1]; CB[2] = CB3[2];
	C[0] = C2[0]; C[1] = C2[1]; C[2] = C2[2];
}

void repeat_rotation(double* p, double* ss, double* cs, double* res)
{
	p[X] = p[X] - shift[X]; p[Y] = p[Y] - shift[Y]; p[Z] = p[Z] - shift[Z];
	double r1[3] = { 0 };
	rotation_z(p, ss[0], cs[0], r1);
	double r2[3] = { 0 };
	rotation_y(r1, -ss[1], cs[1], r2);
	rotation_x(r2, ss[2], cs[2], res);
}

void DumpHex(const void* data, size_t size) {
	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';
	for (i = 0; i < size; ++i) {
		printf("%02X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char*)data)[i];
		}
		else {
			ascii[i % 16] = '.';
		}
		if ((i + 1) % 8 == 0 || i + 1 == size) {
			printf(" ");
			if ((i + 1) % 16 == 0) {
				printf("|  %s \n", ascii);
			}
			else if (i + 1 == size) {
				ascii[(i + 1) % 16] = '\0';
				if ((i + 1) % 16 <= 8) {
					printf(" ");
				}
				for (j = (i + 1) % 16; j < 16; ++j) {
					printf("   ");
				}
				printf("|  %s \n", ascii);
			}
		}
	}
}

double distance(double* a, double* b)
{
	return sqrt(pow(b[0] - a[0], 2) + pow(b[1] - a[1], 2) + pow(b[2] - a[2], 2));
}

int read_ca(char* path, char* text, int size)
{
	FILE* file = fopen(path, "r");
	return fread(text, size, 1, file);
}

int read_file(char* path, char* text, int size)
{
	ZeroMemory(text, size);
	FILE* file = fopen(path, "r");
	int fsize = fread(text, 1, size, file);
	fclose(file);
	return fsize;
}

int get_file_size(char* path)
{
	struct stat st;
	stat(path, &st);
	const int size = st.st_size;
	return size;
}

char* read_pdb(char* path, int* ps)
{
	int size = get_file_size(path);
	char* text = malloc(size);
	*ps = read_file(path, text, size);
	return text;
}

int atom_count(char* pdb_content, int size)
{
	int atoms_size = 0;
	char find[5] = "ATOM\0";
	for (int i = 0; i < size - 5; i++) {
		//printf("%c", pdb_content[i]);
		char fstr[5] = { pdb_content[i], pdb_content[i + 1], pdb_content[i + 2], pdb_content[i + 3], 0 };
		//printf("%s\n", fstr);
		if (
			!strcmp(find, fstr)
			) {
			atoms_size++;
		}
	}
	return atoms_size;
}

struct ATOM {
	char record[7];
	int  atom_id;
	char atom_type[4];
	char alt_loc;
	char mol_name[4];
	char chain_id;
	int res_req;
	char i_code;
	double x;
	double y;
	double z;
	double occupancy;
	double temp_factor;
	char element[3];
	char charge[3];
};

typedef struct ATOM atom;

void print_atom(atom* a)
{
	printf("ATOM: %s MOL: %s X: %lf Y: %lf Z: %lf\n", a->atom_type, a->mol_name, a->x, a->y, a->z);
}

struct PDB {
	int size;
	atom* atoms;
};

char space[10] = { 0 };

typedef struct PDB pdb;

atom parse_pdb_atom_line(char* l) {
	atom a;
	ZeroMemory(a.record, 7); ZeroMemory(a.atom_type, 4); ZeroMemory(a.mol_name, 4); ZeroMemory(a.element, 3); ZeroMemory(a.charge, 3);
	//printf("%s\n", l);

	char 	cx[8] = { l[31], l[32], l[33], l[34], l[35], l[36], l[37], l[38] },
		cy[8] = { l[39], l[40], l[41], l[42], l[43], l[44], l[45], l[46] },
		cz[8] = { l[47], l[48], l[49], l[50], l[51], l[52], l[53], l[54] },
		atom_id[5] = { l[7], l[8], l[9], l[10], l[11] },
		atom_type[5] = { l[13], l[14], l[15], l[16], 0 },
		mol_name[4] = { l[17], l[18], l[19], 0 };

	sscanf(cx, "%lf", &a.x);
	sscanf(cy, "%lf", &a.y);
	sscanf(cz, "%lf", &a.z);
	sscanf(atom_type, "%s", a.atom_type);
	sscanf(mol_name, "%s", a.mol_name);
	sscanf(atom_id, "%i", &a.atom_id);
	//printf("RECORD:\t%i\t%s\t%s\t(%lf,\t%lf,\t%lf)\n", a.atom_id, a.atom_type, a.mol_name, a.x, a.y, a.z);

	return a;
}

atom* parse_atoms(char* pc, int pc_size, int atom_size)
{
	atom* atoms = malloc(sizeof(atom) * atom_size);
	char* token = strtok(pc, "\n");
	char atom_record[7] = "ATOM  \0";
	char complete_record[7] = "ENDMDL\0";
	int cursor = 0;
	while (token != NULL) {
		token = strtok(NULL, "\n");
		if (strlen(token) > 3) {
			//printf("%s\n", token);
			char record[7] = { token[0], token[1], token[2], token[3], token[4], token[5], 0 };
			//printf("AFTER INIT\n");
			if (!strcmp(record, atom_record)) {
				atoms[cursor] = parse_pdb_atom_line(token);
				cursor++;
			}
			else if (!strcmp(record, complete_record)) {
				return atoms;
			}
		}
	}
	return atoms;
}

int count_ca(atom* a, int size)
{
	int ca_count = 0;
	char ca_atom[4] = "CA\0";

	for (int i = 0; i < size; i++) {
		if (!strcmp(a[i].atom_type, ca_atom)) {
			ca_count++;
		}
	}
	return ca_count;
}

int count_ow(atom* a, int size)
{
	int ow_count = 0;
	char ow_atom[4] = "OW\0";

	for (int i = 0; i < size; i++) {
		if (!strcmp(a[i].atom_type, ow_atom)) {
			ow_count++;
		}
	}
	return ow_count;
}

atom* get_all_ca(atom* a, int size)
{

	int ca_count = 0;
	char ca_atom[4] = "CA\0";

	for (int i = 0; i < size; i++) {
		if (!strcmp(a[i].atom_type, ca_atom)) {
			ca_count++;
		}
	}

	atom* CAs = malloc(sizeof(atom) * ca_count);
	int cursor = 0;

	for (int i = 0; i < size; i++) {
		if (!strcmp(a[i].atom_type, ca_atom)) {
			CAs[cursor] = a[i];
			cursor++;
		}
	}
	return CAs;

}

atom* get_all_HOWs(atom* a, int size)
{

	int ow_count = 0;
	char ow_atom[4] = "OW\0";

	for (int i = 0; i < size; i++) {
		if (!strcmp(a[i].atom_type, ow_atom)) {
			ow_count++;
		}
	}

	atom* OWs = malloc(sizeof(atom) * ow_count);
	int cursor = 0;

	for (int i = 0; i < size; i++) {
		if (!strcmp(a[i].atom_type, ow_atom)) {
			OWs[cursor] = a[i];
			cursor++;
		}
	}
	return OWs;

}

atom* get_all_HWs(atom* a, int size, int* counter)
{

	int hw_count = 0;
	char hw_atom1[4] = "HW1\0";
	char hw_atom2[4] = "HW2\0";

	for (int i = 0; i < size; i++) {
		if (!strcmp(a[i].atom_type, hw_atom1) | !strcmp(a[i].atom_type, hw_atom2)) {
			hw_count++;
		}
	}

	*counter = hw_count;
	atom* HWs = malloc(sizeof(atom) * hw_count);
	int cursor = 0;

	for (int i = 0; i < size; i++) {
		if (!strcmp(a[i].atom_type, hw_atom1) | !strcmp(a[i].atom_type, hw_atom2)) {
			HWs[cursor] = a[i];
			cursor++;
		}
	}
	return HWs;

}

void get_HW_by_OW(atom* a, int size, int mol_id, atom* hw1, atom* hw2)
{
	char hw_atom1[4] = "HW1\0";
	char hw_atom2[4] = "HW2\0";
	for (int i = 0; i < size; i++) {
		if (!strcmp(a[i].atom_type, hw_atom1) && a[i].res_req == mol_id) {
			*hw1 = a[i];
		}
		else if (!strcmp(a[i].atom_type, hw_atom2) && a[i].res_req == mol_id) {
			*hw2 = a[i];
		}
	}

}

double atom_distance(atom a, atom b)
{
	double dist = sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2));
	return dist;
}

atom* get_near_ow(atom* ows, atom ca, int ows_size, int* count, atom* near_ow)
{
	double dist = 0;
	int near_ow_count = 0;
	*count = near_ow_count;
	for (int i = 0; i < ows_size; i++) {
		dist = atom_distance(ca, ows[i]);
		if (dist < CA_OW_DISTANCE) {
			near_ow_count++;
		}
	}

	*count = near_ow_count;
	if (near_ow_count == 0) {
		return NULL;
	}
	//near_ow = malloc(sizeof(atom)*near_ow_count);
	if (near_ow == NULL) {
		printf("(532) MALLOC ERROR\n");
		return NULL;
	}
	int cursor = 0;

	for (int i = 0; i < ows_size; i++) {
		dist = atom_distance(ca, ows[i]);
		if (dist < CA_OW_DISTANCE) {
			near_ow[cursor] = ows[i];
			cursor++;
		}
	}
	return near_ow;
}

void get_cb_c_by_res_id(atom* a, int size, int mol_id, atom* cb, atom* c)
{
	char cb_atom_type[3] = "CB\0";
	char h1_atom_type[3] = "H1\0";
	char c_atom_type[2] = "C\0";
	for (int i = 0; i < size; i++) {
		if (a[i].res_req == mol_id && (!strcmp(a[i].atom_type, cb_atom_type) | !strcmp(a[i].atom_type, h1_atom_type))) {
			*cb = a[i];
		}
		else if (a[i].res_req == mol_id && !strcmp(a[i].atom_type, c_atom_type)) {
			*c = a[i];
		}
	}
}

void run_update_points_coordinates(atom* ca, atom* cb, atom* c, double* sin_seq, double* cos_seq)
{
	double CA[3] = { 0 }, CB[3] = { 0 }, C[3] = { 0 };
	CA[X] = ca->x; CB[X] = cb->x; C[X] = c->x;
	CA[Y] = ca->y; CB[Y] = cb->y; C[Y] = c->y;
	CA[Z] = ca->z; CB[Z] = cb->z; C[Z] = c->z;
	update_three_points_coords(CA, CB, C, sin_seq, cos_seq);
}

void run_repeat_rotation(atom* atm, double* sin_sequence, double* cos_sequence)
{
	double atmd[3] = { 0 }, new_atmd[3] = { 0 };
	atmd[X] = atm->x;
	atmd[Y] = atm->y;
	atmd[Z] = atm->z;
	repeat_rotation(atmd, sin_sequence, cos_sequence, new_atmd);
	atm->x = new_atmd[X];
	atm->y = new_atmd[Y];
	atm->z = new_atmd[Z];
}

atom cb, c, hw1, hw2, * phw1, * phw2;
atom near_ows[512];

void file_iteration(char* path, int position)
{
	int pdb_size = 0;
	int* ps = &pdb_size;
	char* content = read_pdb(path, ps);
	int atoms_count = atom_count(content, pdb_size);
	pdb p;
	p.size = atoms_count;
	p.atoms = parse_atoms(content, pdb_size, atoms_count);
	free(content);
	const int ca_size = count_ca(p.atoms, p.size);
	const int ow_size = count_ow(p.atoms, p.size);
	int hw_size = 0;
	int* hw_size_pointer = &hw_size;
	atom* CAs = get_all_ca(p.atoms, p.size);
	atom* HOHs = get_all_HOWs(p.atoms, p.size);
	atom* HWs = get_all_HWs(p.atoms, p.size, hw_size_pointer);
	int near_count = 0;
	int* near_count_pointer = &near_count;
	double sin_sequence[3] = { 0 };
	double cos_sequence[3] = { 0 };
	double ca_o_distance = 0;
	ZeroMemory(&cb, sizeof(atom));
	ZeroMemory(&c, sizeof(atom));
	phw1 = &hw1; phw2 = &hw2;
	//printf("probe 0\n");
	for (int i = 0; i < ca_size; i++) {
		ZeroMemory(near_ows, sizeof(atom) * 512);
		get_near_ow(HOHs, CAs[i], ow_size, near_count_pointer, near_ows);
		if (near_count == 0) {
			continue;
		}
		get_cb_c_by_res_id(p.atoms, p.size, CAs[i].res_req, &cb, &c);
		run_update_points_coordinates(&CAs[i], &cb, &c, sin_sequence, cos_sequence);
		for (int i1 = 0; i1 < near_count; i1++) {
			//printf("probe 1 %i\n", i1);
			if (position == 1 && near_ows[i1].y < 0) {
				continue;
			}
			if (position == -1 && near_ows[i1].y > 0) {
				continue;
			}
			ca_o_distance = atom_distance(CAs[i], near_ows[i1]);
			if (ca_o_distance > CA_OW_DISTANCE) {
				continue;
			}
			//printf("probe 2 %i\n", i1);
			get_HW_by_OW(HWs, hw_size, near_ows[i1].res_req, phw1, phw2);
			//printf("probe 3 %i\n", i1);
			run_repeat_rotation(&near_ows[i], sin_sequence, cos_sequence);
			run_repeat_rotation(phw1, sin_sequence, cos_sequence);
			run_repeat_rotation(phw2, sin_sequence, cos_sequence);
			//printf("probe 4 %i\n", i1);
			phw1->x = phw1->x - near_ows[i1].x;
			phw1->y = phw1->y - near_ows[i1].y;
			phw1->z = phw1->z - near_ows[i1].z;

			phw2->x = phw2->x - near_ows[i1].x;
			phw2->y = phw2->y - near_ows[i1].y;
			phw2->z = phw2->z - near_ows[i1].z;
			//printf("probe 5 %i\n", i1);

			double zenit_h1 = acos(phw1->z / sqrt(pow(phw1->x, 2) + pow(phw1->y, 2) + pow(phw1->z, 2)));
			double azimut_h1 = atan(phw1->y / phw1->x);
			//printf("probe 6 %i\n", i1);

			double zenit_h2 = acos(phw2->z / sqrt(pow(phw2->x, 2) + pow(phw2->y, 2) + pow(phw2->z, 2)));
			double azimut_h2 = atan(phw2->y / phw2->x);
			//printf("probe 7 %i\n", i1);

			printf("%s\t%lf\t%lf\t%lf\t%lf\t%lf\n", CAs[i].mol_name, ca_o_distance, zenit_h1, azimut_h1, zenit_h2, azimut_h2);
		}
		//printf("ON FREE %i %i\n", near_ows, near_count);
		//free(near_ows);
		//printf("AFTER FREE\n");
	}
	free(CAs);
	free(HOHs);
	free(HWs);
	free(p.atoms);
}

char* strtok_r(char* str, const char* delim, char** save)
{
	char* res, * last;


	if (!save)
		return strtok(str, delim);
	if (!str && !(str = *save))
		return NULL;
	last = str + strlen(str);
	if ((*save = res = strtok(str, delim)))
	{
		*save += strlen(res);
		if (*save < last)
			(*save)++;
		else
			*save = NULL;
	}
	return res;
}

char*
strtok1(char* s, const char* delim)
{
	static char* olds;
	return strtok_r(s, delim, &olds);
}

/*int main(int args, char * argv[])
{
	if (args == 1) {
		printf("stat_analyzer <path to start file>\n\Start file is text file which contains paths to pdb descriped time moments of system\n");
	}
	else {
		printf("ARGV[1]: %s\nStart main analytic algorythm", argv[1]);
	}
	char out_file[] = "C:\\users\\softc\\Science\\artifitial_predictor\\source\\start_file.list";
	int position = ON_RADICAL_STAT;
	int size = get_file_size(argv[1]);//(out_file);
	char* text = malloc(size);
	ZeroMemory(text, size);
	read_file(out_file, text, size);
	//printf("%s\n", text);
	char complete[] = "COMPLETE";
	char* token = strtok1(text, "\n");
	while (token != NULL) {
		file_iteration(token, position);
		token = strtok1(NULL, "\n");
		if (!strcmp(token, complete)) {
			break;
		}
		//printf("FILE: %s\n", token);
	}
	free(text);

	return 0;
}*/