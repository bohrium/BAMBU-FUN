#include <stdio.h>

#include "stl.h"



#define STL_HEADER_SIZE 80
#define STL_NBTRIS_SIZE  4
#define STL_SPACER_SIZE  2



void write_stl_to(FILE* fp, stl_t s, char const* model_name)
{
    char buff[STL_HEADER_SIZE];
    for (int i=0; i!=STL_HEADER_SIZE; i++) {
        buff[i] = model_name[i];
        if ( ! model_name[i] ) { break; }
    }
    fwrite(buff, 1, STL_HEADER_SIZE, fp);

    // little-endian
    int nt = s.nb_tris;
    for (int i=0; i!=STL_NBTRIS_SIZE; i++) {
        fputc(nt%(1<<8), fp);
        nt /= (1<<8);
    }

    vec_t dummy_normal = (vec_t){0,0,0};
    for (int t=0; t!=s.nb_tris; t++) {
        fwrite(&dummy_normal, sizeof(vec_t), 1, fp);
        fwrite(&(s.tri[t].a), sizeof(vec_t), 1, fp);
        fwrite(&(s.tri[t].b), sizeof(vec_t), 1, fp);
        fwrite(&(s.tri[t].c), sizeof(vec_t), 1, fp);
        for (int i=0; i!=STL_SPACER_SIZE; ++i) { fputc(0, fp); }
    }
}


