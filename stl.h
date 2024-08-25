/* author: samtenka
 * change: 2024-08-23
 * create: 2024-08-23
 * descrp: TODO
 * to use: TODO
 */

#include <stdio.h>



#define MAX_TRIS 1000000



#ifndef STL_H
#define STL_H



typedef struct { float x, y, z; } vec_t;

typedef struct { vec_t a, b, c; } tri_t;

typedef struct {
    int nb_tris;
    tri_t tri[MAX_TRIS];
} stl_t;



void init_stl(stl_t* s);

void add_tri(stl_t* s, tri_t t);

void make_fourier(float_t* vals, int resolution, float_t const coeffs[][2], int nb_coeffs);

void add_tube(
// assumes centers is generic (so normal and binormal vectors always well defined)
    stl_t* s,
    vec_t const* centers, int nb_centers,
    int cross_section_resolution,
    float_t const* coeffs_normal,
    float_t const* coeffs_binorm
);

void add_sheet(stl_t* s, int R, int C,
        vec_t const* over,
        vec_t const* under
        );

void write_stl_to(FILE* fp, stl_t const* s, char const* model_name);



#endif//STL_H
