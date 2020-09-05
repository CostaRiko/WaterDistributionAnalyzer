#include "protein_based_calculations.h"

/*
Main function is calculate_water_orientational_distribution
params:
    CA, C alpha coordinates vector
    CB, C beta coordinates vector
    N,  N coordinates vector
    HOHa, All hoh array
    HOHs, HOH coordinates array pointer
    distance: maximal distance from CA to O atom in A
return:
    int len : length of result coordinates array

*/

void rotation_x(vector* v, double cosa, double sina)
{
    v->y = v->y * cosa - v->z * sina;
    v->z = v->y * sina + v->z * cosa;
}

void rotation_y(vector* v, double cosa, double sina)
{
    /*x*/ v->x = v->x * cosa + v->z * sina;
    /*z*/ v->z = (-1) * v->x * sina + v->z * cosa;
}

void rotation_z(vector* v, double cosa, double sina)
{
    /*x*/ v->x = v->x * cosa - v->y * sina;
    /*y*/ v->y = v->x * sina + v->y * cosa;
}

void shift(vector* shift, vector* v)
{
    v->x = v->x - shift->x;
    v->y = v->y - shift->y;
    v->z = v->z - shift->z;
}

double distance(vector a, vector b)
{
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}

double cos_a(vector* v1, vector* v2)
{
    double divident = (v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z);
    double divider = (pow(v1->x, 2) + pow(v1->y, 2) + pow(v1->z, 2)) * ( pow(v2->x, 2) + pow(v2->y, 2) + pow(v2->z, 2) );
    return  divident / divider;
}

int calculate_water_orientational_distribution(vector CA, vector CB, vector N, vector* HOHa, int HOHa_len, double dist)
{
    vector x_direction = { 1, 0, 0 }, y_direction = { 0, 1, 0 };

    //check which hohs in distance area
    int near_hoh_counter = 0;
    for (int i = 0; i < HOHa_len; i++) {
        if ( distance(HOHa[i], CA) < dist ) {
            near_hoh_counter++;
        }
    }

    //Shift CA to coordinates origin. shift all other atoms to origin_shift

    shift(&CA, &CB);
    shift(&CA, &N);

    vector cb_oxy_proj = { CB.x, CB.y, 0 };
    step steps[3];

    double cosa, sina;

    // cosa CB - X

    cosa = cos_a(&cb_oxy_proj, &x_direction);
    sina = sin_from_cos(cosa);
    if (cb_oxy_proj.y < 0) {
        sina = -sina;
    }

    steps[0].sina = sina;
    steps[0].cosa = cosa;
    rotation_z(&CB, cosa, sina);
    rotation_z(&N, cosa, sina);

    // cosa CB` - X
    cosa = cos_a(&CB, &x_direction);
    sina = sin_from_cos(cosa);
    if (CB.z > 0) {
        sina = -sina;
    }

    steps[1].sina = sina;
    steps[1].cosa = cosa;
    rotation_y(&CB, cosa, sina);
    rotation_y(&N, cosa, sina);
    
    // cosa N`` - X
    vector n_yoz_proj = { 0, N.y, N.z };
    cosa = cos_a(&N, &n_yoz_proj);
    sina = sin_from_cos(cosa);
    if (N.z < 0) {
        sina = -sina;
    }

    steps[2].sina = sina;
    steps[2].cosa = cosa;
    rotation_x(&N, cosa, sina);

    vector* HOH_s = malloc(sizeof(vector) * near_hoh_counter);

    for (int i = 0; i < HOHa_len; i++) {
        if (distance(HOHa[i], CA) < dist) {
            CopyMemory(&HOH_s[i], &HOHa[i], sizeof(vector));
            shift(&CA, &HOH_s[i]);
            rotation_z(&HOH_s[i], steps[0].cosa, steps[0].sina);
            rotation_y(&HOH_s[i], steps[1].cosa, steps[1].sina);
            rotation_x(&HOH_s[i], steps[2].cosa, steps[2].sina);
        }
    }
    free(HOH_s);
    print_vector(&CB);
    print_vector(&N);
    return 0;
}

double sin_from_cos(double cosa)
{
    return sqrt(1 - pow(cosa, 2));
}

void print_vector(vector* a) 
{
    printf("VECTOR: x = %lf y = %lf z = %lf\n", a->x, a->y, a->z);
}