#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "menu.h"
#include "z3D/z3D.h"

typedef struct FreeCam {
    u8 enabled;
    u8 locked;
    u8 mode;
    u8 behavior;
    Vec3f eye;
    Vec3f at;
    Vec3s rot;
    s16 dist;
    u8 hasStoredView;
    View storedViewData;
    u8 rememberPos;
} FreeCam;

enum FreeCamBehavior {
    CAMBHV_MANUAL,
    // CAMBHV_BIRDSEYE,
    CAMBHV_RADIAL,
};

enum FreeCamMode {
    CAMMODE_CAMERA,
    CAMMODE_VIEW,
};

extern FreeCam freeCam;

#define FreeCam_Moving (freeCam.enabled && !freeCam.locked)

void FreeCam_Update(void);
void FreeCam_Toggle(void);
void FreeCam_ToggleLock(void);

#endif //_CAMERA_H_
