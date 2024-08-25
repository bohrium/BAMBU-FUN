#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "stl.h"



#define STL_HEADER_SIZE 80
#define STL_NBTRIS_SIZE  4
#define STL_SPACER_SIZE  2



void make_fourier(float_t* vals, int resolution, float_t const coeffs[][2], int nb_coeffs)
{
    for (int k=0; k!=resolution; ++k) {
        vals[k] = 0.;
        float om = 2*M_PI * (float)k/resolution;
        for (int i=0; i!=nb_coeffs; ++i) {
            vals[k] += (
                    cos((1+i)*om) * coeffs[i][0]
                  + sin((1+i)*om) * coeffs[i][1]
            );
        }
    }
}



void init_stl(stl_t* s)
{
    s->nb_tris = 0;
}



void add_tri(stl_t* s, tri_t t)
{
    s->tri[s->nb_tris] = t;
    s->nb_tris += 1;
}



vec_t add_scale(vec_t a, float scale, vec_t b)
{
    return (vec_t){
        a.x + scale * b.x,
        a.y + scale * b.y,
        a.z + scale * b.z,
    };
}



vec_t add_scales(float scale_a, vec_t a, float scale_b, vec_t b)
{
    return (vec_t){
        scale_a * a.x + scale_b * b.x,
        scale_a * a.y + scale_b * b.y,
        scale_a * a.z + scale_b * b.z,
    };
}



vec_t cross(vec_t a, vec_t b)
{
    return (vec_t){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
}



vec_t normalized(vec_t a)
{
    float norm = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
    return (vec_t){
        a.x / norm,
        a.y / norm,
        a.z / norm,
    };
}



void add_tube(
    stl_t* s,
    vec_t const* centers, int nb_centers,
    int cross_section_resolution,
    float_t const* coeffs_normal,
    float_t const* coeffs_binorm
)
{
    vec_t* normals = malloc(sizeof(vec_t)*nb_centers);
    vec_t* binorms = malloc(sizeof(vec_t)*nb_centers);
    for (int r=0; r!=nb_centers; ++r) {
        vec_t vel_plus = add_scale(centers[(r+1)%nb_centers], -1., centers[(r)%nb_centers]);
        vec_t vel_minus= add_scale(centers[(r)%nb_centers], -1., centers[(r+nb_centers-1)%nb_centers]);
        vec_t tangent = normalized(add_scale(vel_plus, +1., vel_minus));
        vec_t normal = normalized(add_scale(vel_plus, -1., vel_minus));
        vec_t binorm = cross(tangent, normal);

        normals[r] = normal;
        binorms[r] = binorm;
    }

    for (int r=0; r!=nb_centers; ++r) {
        for (int c=0; c!=cross_section_resolution; ++c) {
            vec_t quad_corners[2][2];

            for (int dr=0; dr!=2; ++dr) {
                for (int dc=0; dc!=2; ++dc) {
                    quad_corners[dr][dc] = add_scale(
                            centers[(r+dr)%nb_centers],
                            1., add_scales(
                                coeffs_normal[(c+dc)%cross_section_resolution], normals[(r+dr)%nb_centers],
                                coeffs_binorm[(c+dc)%cross_section_resolution], binorms[(r+dr)%nb_centers]
                            )
                    );
                }
            }
            vec_t v00 = quad_corners[0][0];
            vec_t v01 = quad_corners[0][1];
            vec_t v10 = quad_corners[1][0];
            vec_t v11 = quad_corners[1][1];

            // printf("%6.3f %6.3f %6.3f\n", v00.x, v00.y, v00.z);

            add_tri(s, (tri_t){v00,v01,v10});
            add_tri(s, (tri_t){v01,v10,v11});
        }
    }

    free(normals);
    free(binorms);
}



void write_stl_to(FILE* fp, stl_t const* s, char const* model_name)
{
    char buff[STL_HEADER_SIZE];
    for (int i=0; i!=STL_HEADER_SIZE; i++) {
        buff[i] = model_name[i];
        if ( ! model_name[i] ) { break; }
    }
    fwrite(buff, 1, STL_HEADER_SIZE, fp);

    // little-endian
    int nt = s->nb_tris;
    for (int i=0; i!=STL_NBTRIS_SIZE; i++) {
        fputc(nt%(1<<8), fp);
        nt /= (1<<8);
    }

    vec_t dummy_normal = (vec_t){0,0,0};
    for (int t=0; t!=s->nb_tris; t++) {
        fwrite(&dummy_normal, sizeof(vec_t), 1, fp);
        fwrite(&(s->tri[t].a), sizeof(vec_t), 1, fp); // note: can do all at once
        fwrite(&(s->tri[t].b), sizeof(vec_t), 1, fp); //
        fwrite(&(s->tri[t].c), sizeof(vec_t), 1, fp); //
        for (int i=0; i!=STL_SPACER_SIZE; ++i) { fputc(0, fp); }
    }
}


void add_sheet(stl_t* s, int R, int C,
        vec_t const* over,
        vec_t const* under
        )
{
    vec_t v00,v01,v10,v11;

    for (int r=0; r!=R-1; ++r) {
        for (int c=0; c!=C-1; ++c) {
            v00 = over[(r+0)*C+(c+0)];
            v01 = over[(r+0)*C+(c+1)];
            v10 = over[(r+1)*C+(c+0)];
            v11 = over[(r+1)*C+(c+1)];
            add_tri(s, (tri_t){v00,v01,v10});
            add_tri(s, (tri_t){v01,v10,v11});

            v00 = under[(r+0)*C+(c+0)];
            v01 = under[(r+0)*C+(c+1)];
            v10 = under[(r+1)*C+(c+0)];
            v11 = under[(r+1)*C+(c+1)];
            add_tri(s, (tri_t){v00,v01,v10});
            add_tri(s, (tri_t){v01,v10,v11});
        }
    }

    for (int r=0; r!=R-1; ++r) {
        v00 = over[(r+0)*C+0];
        v01 =under[(r+0)*C+0];
        v10 = over[(r+1)*C+0];
        v11 =under[(r+1)*C+0];
        add_tri(s, (tri_t){v00,v01,v10});
        add_tri(s, (tri_t){v01,v10,v11});

        v00 = over[(r+0)*C+C-1];
        v01 =under[(r+0)*C+C-1];
        v10 = over[(r+1)*C+C-1];
        v11 =under[(r+1)*C+C-1];
        add_tri(s, (tri_t){v00,v01,v10});
        add_tri(s, (tri_t){v01,v10,v11});
    }

    for (int c=0; c!=C-1; ++c) {
        v00 = over[0*C+(c+0)];
        v01 = over[0*C+(c+1)];
        v10 =under[0*C+(c+0)];
        v11 =under[0*C+(c+1)];
        add_tri(s, (tri_t){v00,v01,v10});
        add_tri(s, (tri_t){v01,v10,v11});

        v00 = over[(R-1)*C+(c+0)];
        v01 = over[(R-1)*C+(c+1)];
        v10 =under[(R-1)*C+(c+0)];
        v11 =under[(R-1)*C+(c+1)];
        add_tri(s, (tri_t){v00,v01,v10});
        add_tri(s, (tri_t){v01,v10,v11});
    }
}


