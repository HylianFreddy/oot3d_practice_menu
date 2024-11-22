#ifndef _COLOR_H
#define _COLOR_H

#include "z3D/z3Dvec.h"

typedef struct {
    u8 r, g, b;
} Color_RGB8;

typedef struct {
    u8 r, g, b, a;
} Color_RGBA8;

typedef union {
    struct {
        u8 a, b, g, r;
    };
    u32 rgba;
} Color_RGBA8_u32;

typedef struct {
    f32 r, g, b, a;
} Color_RGBAf;

#endif //_COLOR_H
