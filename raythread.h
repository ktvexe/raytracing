#ifndef RAYTHREAD_H
#define RAYTHREAD_H

#include"objects.h"
#include "primitives.h"
struct parameter {
    uint8_t *pixels;
    light_node lights ;
    rectangular_node rectangulars ;
    sphere_node spheres ;
    color background ;
    const viewpoint *view;
    int begin_col;
    //int begin_h;
    int finish_col;
    //int finish_h;
    int  width;
    int height;
};
#endif
