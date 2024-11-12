#ifndef _COLVIEW_H_
#define _COLVIEW_H_

#include "z3D/z3D.h"
#include "menu.h"

enum ColViewOptions {
    COLVIEW_SHOW_COLLIDERS,
    COLVIEW_AT,
    COLVIEW_AC,
    COLVIEW_OC,
    COLVIEW_SHOW_COLLISION,
    COLVIEW_STATIC,
    COLVIEW_DYNAMIC,
    COLVIEW_INVISIBLE_SEAMS,
    COLVIEW_TRANSLUCENT,
    COLVIEW_POLYGON_CLASS,
    COLVIEW_SHADED,
    COLVIEW_REDUCED,
    COLVIEW_ADVANCED_OPTIONS_MENU,
    COLVIEW_MAX,
};

typedef struct ColViewPoly {
    Vec3f verts[3];
    Vec3f norm;
    f32 dist;
    Color_RGBAf color;
} ColViewPoly;

extern u8 gColViewDisplayCountInfo;
extern ToggleMenu CollisionMenu;
#define CollisionOption(option) (CollisionMenu.items[option].on)

void ColView_CollisionMenuShow(void);
void ColView_DrawCollision(void);

#endif // _COLVIEW_H_
