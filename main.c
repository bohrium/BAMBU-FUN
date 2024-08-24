#include <math.h>
#include <stdlib.h>

#include "stl.h"



void test_write_stl();
void test_write_torus(int maj_res, int min_res, float maj_rad, float min_rad);



int main(int argc, char** argv)
{
    //test_write_stl();
    test_write_torus(60, 60, 5., 2.);
    return 0;
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
    stl_t s;
    init_stl(&s);

    for (int r=0; r!=maj_res; ++r) {
        for (int c=0; c!=min_res; ++c) {
            vec_t v00 = vert_from(((float)r+0.)/maj_res, ((float)c+0.)/min_res, maj_rad, min_rad);
            vec_t v01 = vert_from(((float)r+0.)/maj_res, ((float)c+1.)/min_res, maj_rad, min_rad);
            vec_t v10 = vert_from(((float)r+1.)/maj_res, ((float)c+0.)/min_res, maj_rad, min_rad);
            vec_t v11 = vert_from(((float)r+1.)/maj_res, ((float)c+1.)/min_res, maj_rad, min_rad);
            add_tri(&s, (tri_t){v00,v01,v10});
            add_tri(&s, (tri_t){v01,v10,v11});
            //s.tri[s.nb_tris] = (tri_t){v00,v01,v10};
            //s.nb_tris += 1;
            //s.tri[s.nb_tris] = (tri_t){v01,v10,v11};
            //s.nb_tris += 1;
        }
    }

    printf("%d tris\n", s.nb_tris);

    FILE* fp = fopen("torus.stl", "wb");
    write_stl_to(fp, s, "mootorus");
    fclose(fp);
}


void test_write_stl()
{
    stl_t s;
    s.nb_tris = 4;
    s.tri[0] = (tri_t){{2,1,1},{1,2,1},{1,1,2}};
    s.tri[1] = (tri_t){{3,3,3},{1,2,1},{1,1,2}};
    s.tri[2] = (tri_t){{2,1,1},{3,3,3},{1,1,2}};
    s.tri[3] = (tri_t){{2,1,1},{1,2,1},{3,3,3}};

    FILE* fp = fopen("moo.stl", "wb");
    write_stl_to(fp, s, "moomoo");
    fclose(fp);
}
