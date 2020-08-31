#ifndef PROTEIN_BASED
#define PROTEIN_BASED 1
#include <math.h>

void rotation_x(double* v, double cosa, double sina);
void rotation_y(double* v, double cosa, double sina);
void rotation_z(double* v, double cosa, double sina);
void shift( double * shift, double * vector);
double distance(double * a, double * b);
double cos_a(double * a, double * b);
double get_matrix_dimensions(double* m1, int * x, int * y);

#endif