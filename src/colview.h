#ifndef _COLVIEW_H_
#define _COLVIEW_H_

#include "z3D/z3D.h"

typedef struct ColViewPoly {
    Vec3f vA;
    Vec3f vB;
    Vec3f vC;
    Vec3s norm;
    Color_RGBAf color;
} ColViewPoly;

void ColView_DrawCollision(void);

#endif // _COLVIEW_H_
