#include "protein_based_calculations.h"

void rotation_x(double* v, double cosa, double sina)
{
    v[1] = v[1] * cosa - v[2] * sina;
    v[2] = v[1] * sina + v[2] * cosa;
}

void rotation_y(double* v, double cosa, double sina)
{
    /*x*/ v[0] = v[0] * cosa + v[2] * sina;
    /*z*/ v[2] = (-1) * v[0] * sina + v[2] * cosa;
}

void rotation_z(double* v, double cosa, double sina)
{
    /*x*/ v[0] = v[0] * cosa - v[1] * sina;
    /*y*/ v[1] = v[0] * sina + v[1] * cosa;
}

void shift(double* shift, double* vector)
{
    vector[0] = vector[0] - shift[0];
    vector[1] = vector[1] - shift[1];
    vector[2] = vector[2] - shift[2];
}

double distance(double* a, double* b)
{
    return sqrt(pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2) + pow(a[2] - b[2], 2));
}

double cos_a(double* v1, double* v2)
{
    double divident = (v1[0] * v2[0]) + (v1[1] * v2[2]) + (v1[2] * v2[2]);
    double divider = (pow(v1[0], 2) + pow(v1[1], 2) + pow(v1[2], 2)) * ( pow(v2[0], 2) + pow(v2[1], 2) + pow(v2[2], 2) );
    return  divident / divider;
}

double sin_from_cos(double cosa)
{
    return sqrt(1 - pow(cosa, 2));
}