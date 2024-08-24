#include <math.h>
#include <stdlib.h>

#include "stl.h"



stl_t s;

void test_write_stl();

void test_write_torus(int maj_res, int min_res, float maj_rad, float min_rad);

void test_write_tube(int maj_res, int min_res, float maj_rad, float min_rad);

void test_write_sheet();



int main(int argc, char** argv)
{
    //test_write_stl();
    //test_write_torus(60, 60, 5., 2.);
    //test_write_tube(150, 90, 5., 2.);
    test_write_sheet();
    return 0;
}



void test_write_sheet()
{
    init_stl(&s);

    int R=60;
    int C=60;
    vec_t over[60*60];
    vec_t under[60*60];
    for (int r=0; r!=60; ++r) {
        for (int c=0; c!=60; ++c) {
            float rr = 0.1*r;
            float cc = 0.1*c;
            over[r*C+c]  = (vec_t){rr, cc, 1.1 + sin(rr+2*cc)};
            under[r*C+c] = (vec_t){rr, cc, 1.0 + sin(rr+2*cc)};
        }
    }

    add_sheet(&s, R, C, over, under);

    FILE* fp = fopen("sheet.stl", "wb");
    write_stl_to(fp, s, "moosheet");
    fclose(fp);
}



void test_write_tube(int maj_res, int min_res, float maj_rad, float min_rad)
{
    init_stl(&s);

    int nb_centers = maj_res;
    vec_t* centers = malloc(sizeof(vec_t)*nb_centers);

    float* coeffs_normal = malloc(sizeof(float)*min_res);
    float* coeffs_binorm = malloc(sizeof(float)*min_res);
    make_fourier(coeffs_normal, min_res, (const float[3][2]){{+2. , 0. },{+1.0, 0. },{-0.2, 0. }}, 3);
    make_fourier(coeffs_binorm, min_res, (const float[3][2]){{ 0. ,+2. },{ 0. , 0. },{ 0. , 0. }}, 3);

    float* centers_transpose[3];
    centers_transpose[0] = malloc(sizeof(float)*nb_centers);
    centers_transpose[1] = malloc(sizeof(float)*nb_centers);
    centers_transpose[2] = malloc(sizeof(float)*nb_centers);
    make_fourier(centers_transpose[0], nb_centers,
        (const float[5][2]){
            {+5.000, 0.   },
            { 0.000, 0.   },
            { 0.025, 0.   },
            {-0.025, 0.   },
            {+0.025, 0.   },
        },
        5
    );
    make_fourier(centers_transpose[1], nb_centers,
        (const float[5][2]){
            { 0.   ,+5.000},
            { 0.150, 0.   },
            { 0.075, 0.   },
            { 0.   , 0.   },
            {+0.025, 0.   },
        },
        5
    );
    make_fourier(centers_transpose[2], nb_centers,
        (const float[5][2]){
            { 0.   , 0.   },
            { 0.   ,+0.025},
            { 0.   ,+0.025},
            { 0.   ,+0.025},
            { 0.   ,+0.125},
        },
        5
    );
    for (int r=0; r!=nb_centers; ++r) {
        centers[r] = (vec_t){
            centers_transpose[0][r],
            centers_transpose[1][r],
            centers_transpose[2][r],
        };
    }
    free(centers_transpose[0]);
    free(centers_transpose[1]);
    free(centers_transpose[2]);


    add_tube(
        &s,
        centers, nb_centers,
        min_res,
        coeffs_normal,
        coeffs_binorm
    );

    free(centers);

    FILE* fp = fopen("tube.stl", "wb");
    write_stl_to(fp, s, "mooloop");
    fclose(fp);
}



vec_t vert_from(float rr, float cc, float maj_rad, float min_rad)
{
    return (vec_t){
        cos(2*M_PI * rr) * (maj_rad + cos(2*M_PI * cc) * min_rad)   ,
        sin(2*M_PI * rr) * (maj_rad + cos(2*M_PI * cc) * min_rad)   ,
        sin(2*M_PI * cc) * min_rad                                  ,
    };
}

void test_write_torus(int maj_res, int min_res, float maj_rad, float min_rad)
{
    init_stl(&s);

    for (int r=0; r!=maj_res; ++r) {
        for (int c=0; c!=min_res; ++c) {
            vec_t v00 = vert_from(((float)r+0.)/maj_res, ((float)c+0.)/min_res, maj_rad, min_rad);
            vec_t v01 = vert_from(((float)r+0.)/maj_res, ((float)c+1.)/min_res, maj_rad, min_rad);
            vec_t v10 = vert_from(((float)r+1.)/maj_res, ((float)c+0.)/min_res, maj_rad, min_rad);
            vec_t v11 = vert_from(((float)r+1.)/maj_res, ((float)c+1.)/min_res, maj_rad, min_rad);
            add_tri(&s, (tri_t){v00,v01,v10});
            add_tri(&s, (tri_t){v01,v10,v11});
        }
    }

    printf("%d tris\n", s.nb_tris);

    FILE* fp = fopen("torus.stl", "wb");
    write_stl_to(fp, s, "mootorus");
    fclose(fp);
}


void test_write_stl()
{
    s.nb_tris = 4;
    s.tri[0] = (tri_t){{2,1,1},{1,2,1},{1,1,2}};
    s.tri[1] = (tri_t){{3,3,3},{1,2,1},{1,1,2}};
    s.tri[2] = (tri_t){{2,1,1},{3,3,3},{1,1,2}};
    s.tri[3] = (tri_t){{2,1,1},{1,2,1},{3,3,3}};

    FILE* fp = fopen("moo.stl", "wb");
    write_stl_to(fp, s, "moomoo");
    fclose(fp);
}
