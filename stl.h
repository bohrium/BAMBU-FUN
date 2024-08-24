/* author: samtenka
 * change: 2024-08-23
 * create: 2024-08-23
 * descrp: TODO
 * to use: TODO
 */

#include <stdio.h>



#define MAX_TRIS 100000



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

void write_stl_to(FILE* fp, stl_t s, char const* model_name);




#endif//STL_H
