#ifndef _COLVIEW_H_
#define _COLVIEW_H_

#include "z3D/z3D.h"

typedef struct ColViewPoly {
    Vec3f verts[3];
    Vec3f norm;
    f32 dist;
    Color_RGBAf color;
} ColViewPoly;

void ColView_DrawCollision(void);

#endif // _COLVIEW_H_
