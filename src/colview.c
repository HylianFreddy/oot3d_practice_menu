#include "colview.h"

void ColView_DrawCollision(void) {
    Vec3f vA = {
        .x=0.0f,
        .y=50.0f,
        .z=0.0f,
    };
    Vec3f vB = {
        .x=50.0f,
        .y=0.0f,
        .z=0.0f,
    };
    Vec3f vC = {
        .x=0.0f,
        .y=0.0f,
        .z=50.0f,
    };
    Color_RGBAf color = {
        .r = 1.0f,
        .g = 0.0f,
        .b = 1.0f,
        .a = 1.0f,
    };
    if (gGlobalContext->cameraPtrs[gGlobalContext->activeCamera]->camDir.y > 0) {
        Collider_DrawPolyImpl((void*)0x5c1858, &vA, &vB, &vC, &color);
    }
}
