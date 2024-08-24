#include <stdlib.h>

#include "stl.h"



void test_write_stl();



int main(int argc, char** argv)
{
    test_write_stl();
    return 0;
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




