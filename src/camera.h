#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "menu.h"
#include "z3D/z3D.h"

typedef struct FreeCam {
    u8 enabled;
    u8 locked;
    u8 behavior;
    // u8 viewMode;
    View storedView;
    Vec3f eye;
    Vec3f at;
    Vec3s rot;
    s16 dist;
} FreeCam;

enum FreeCamBehavior {
    CAMBHV_MANUAL,
    // CAMBHV_BIRDSEYE,
    CAMBHV_RADIAL,
};

extern FreeCam freeCam;

#define FreeCam_Moving (freeCam.enabled && !freeCam.locked)

void FreeCam_Update(void);
void FreeCam_Toggle(void);
void FreeCam_ToggleLock(void);

#endif //_CAMERA_H_
