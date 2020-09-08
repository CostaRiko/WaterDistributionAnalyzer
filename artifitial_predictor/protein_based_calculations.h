#ifndef PROTEIN_BASED
#define PROTEIN_BASED 1
#include <math.h>
#include <Windows.h>

typedef struct {
	double x;
	double y; 
	double z;
} vector;

typedef struct {
	double sina;
	double cosa;
} step;

int atoms_count, atoms_cursor;
int water_size, water_cursor;
int protein_size, protein_cursor;

void rotation_x(vector* v, double cosa, double sina);
void rotation_y(vector* v, double cosa, double sina);
void rotation_z(vector* v, double cosa, double sina);
void shift( vector * shift, vector * vector);
double distance(vector a, vector b);
double check_if_null(double val);
double cos_a(vector * a, vector * b);
/*
	return 
*/
int calculate_water_orientational_distribution(
	vector CA, 
	vector CB, 
	vector N, 
	vector* HOHa, 
	int HOHa_len, 
	double dist
);
double sin_from_cos(double cosa);
void print_vector(vector* a);

#endif