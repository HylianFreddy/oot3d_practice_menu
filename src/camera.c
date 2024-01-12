#include "menu.h"
#include "menus/scene.h"
#include "draw.h"
#include "z3D/z3D.h"
#include "common.h"
#include "input.h"
#include "camera.h"

FreeCam freeCam = { 0 };

f32 sqrtf(f32 x) {
    f32 n = (1 + x) * 0.5;

    while (n * n < x * 0.999f || n * n > x * 1.001f) {
        n = (n + x / n) * 0.5;
    }

    return n;
}

f32 distXYZ(Vec3f a, Vec3f b) {
    f32 x = a.x - b.x, y = a.y - b.y, z = a.z - b.z;
    return sqrtf(x * x + y * y + z * z);
}

s16 Clamp(s16 val) {
    if (val >= 0x3F00)
        return 0x3F00;
    if (val <= -0x3F00)
        return -0x3F00;
    return val;
}

u8 FreeCam_IsEnabled(void) {
    return freeCam.enabled;
}

void FreeCam_Toggle(void) {
    if (!isInGame()) {
        return;
    }

    if (noClip) {
        setAlert("NoClip is active!", 90);
        return;
    }

    if (!freeCam.enabled) {
        freeCam.enabled    = 1;
        freeCam.storedView = gGlobalContext->view;
        freeCam.eye        = gGlobalContext->view.eye;
        freeCam.at         = gGlobalContext->view.at;
        freeCam.rot        = gGlobalContext->cameraPtrs[gGlobalContext->activeCamera]->camDir;
        freeCam.dist       = (s16)distXYZ(freeCam.eye, freeCam.at);
        haltActors         = !freeCam.locked;
    } else {
        gGlobalContext->view = freeCam.storedView;
        haltActors           = 0;
        freeCam.enabled      = 0;
    }
}

void FreeCam_ToggleLock(void) {
    freeCam.locked ^= 1;
    if (freeCam.enabled) {
        haltActors = !freeCam.locked;
    }
}

static void FreeCam_Manual(void) {
    u32 in                = rInputCtx.cur.val;
    circlePosition cStick = rInputCtx.cStick;
    f32 posMult           = (in & BUTTON_X) ? 0.8 : 0.16;
    u16 rotMult           = (in & BUTTON_X) ? 10 : 4;
    Vec3f* eye            = &freeCam.eye;
    Vec3s* rot            = &freeCam.rot;
    u8 usingCStick        = cStick.dx * cStick.dx + cStick.dy * cStick.dy > 100;
    u8 usingCirclePad     = ControlStick_X * ControlStick_X + ControlStick_Y * ControlStick_Y > 100;

    if (!freeCam.locked) {
        if (in & BUTTON_L1 && !usingCStick && usingCirclePad) {
            rot->x += ControlStick_Y * rotMult * 2;
            rot->y -= ControlStick_X * rotMult * 2;
        } else if (usingCirclePad) {
            eye->x += posMult * (ControlStick_Y * sins(rot->y) * coss(rot->x) - ControlStick_X * coss(rot->y));
            eye->y += posMult * (ControlStick_Y * sins(rot->x));
            eye->z += posMult * (ControlStick_Y * coss(rot->y) * coss(rot->x) + ControlStick_X * sins(rot->y));
        }

        if (usingCStick) {
            rot->x += cStick.dy * rotMult;
            rot->y -= cStick.dx * rotMult * ((gSaveContext.masterQuestFlag) ? -1 : 1);
        }

        rot->x = Clamp(rot->x);

        if (in & BUTTON_UP) {
            eye->y += 50 * posMult;
        }
        if (in & BUTTON_DOWN) {
            eye->y -= 50 * posMult;
        }
    }

    freeCam.at.x = eye->x + coss(rot->x) * sins(rot->y);
    freeCam.at.y = eye->y + sins(rot->x);
    freeCam.at.z = eye->z + coss(rot->x) * coss(rot->y);
}

static void FreeCam_Radial(void) {
    u32 in       = rInputCtx.cur.val;
    s8 speedRot  = 16;
    s8 speedDist = (in & BUTTON_X) ? 30 : 8;

    if (!freeCam.locked) {
        freeCam.rot.y += ControlStick_X * speedRot * ((gSaveContext.masterQuestFlag) ? -1 : 1);
        freeCam.rot.x = Clamp(freeCam.rot.x - ControlStick_Y * speedRot);

        if (in & BUTTON_DOWN) {
            freeCam.dist += speedDist;
        }
        if (in & BUTTON_UP) {
            freeCam.dist -= speedDist;
            freeCam.dist = MAX(freeCam.dist, 10);
        }
    }

    freeCam.at = PLAYER->actor.world.pos;
    freeCam.at.y += ((gSaveContext.linkAge) ? 38 : 55);

    freeCam.eye.x = freeCam.at.x - freeCam.dist * coss(freeCam.rot.x) * sins(freeCam.rot.y);
    freeCam.eye.y = freeCam.at.y - freeCam.dist * sins(freeCam.rot.x);
    freeCam.eye.z = freeCam.at.z - freeCam.dist * coss(freeCam.rot.x) * coss(freeCam.rot.y);
}

static void FreeCam_Move(void) {
    if (freeCam.behavior == CAMBHV_MANUAL) {
        FreeCam_Manual();
    } else {
        FreeCam_Radial();
    }
}

void FreeCam_Update(void) {
    if (!isInGame()) {
        return;
    }

    FreeCam_Move();

    View* view           = &gGlobalContext->view;
    Camera* activeCamera = gGlobalContext->cameraPtrs[gGlobalContext->activeCamera];

    view->eye = freeCam.eye;
    view->at  = freeCam.at;

    view->up.x = 0;
    view->up.y = 1;
    view->up.z = 0;

    view->distortionOrientation.x = 0;
    view->distortionOrientation.y = 0;
    view->distortionOrientation.z = 0;

    view->distortionScale.x = 1;
    view->distortionScale.y = 1;
    view->distortionScale.z = 1;

    activeCamera->inputDir = freeCam.rot;
    activeCamera->camDir   = freeCam.rot;

    if (!freeCam.locked) {
        u32 in = rInputCtx.cur.val;
        if (in & BUTTON_B) {
            FreeCam_Toggle();
        } else if (in & BUTTON_A) {
            FreeCam_ToggleLock();
        }
    }
}
