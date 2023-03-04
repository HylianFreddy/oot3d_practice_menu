#include "z3D/z3D.h"
#include "actors/player.h"
#include "draw.h"
#include "input.h"

ActorInit vanillaActorInit_Player = {0};
void start_loop();

void PlayerActor_rInit(Actor* thisx, GlobalContext* globalCtx) {
    vanillaActorInit_Player.init(thisx, globalCtx);
}

void PlayerActor_rUpdate(Actor* thisx, GlobalContext* globalCtx) {
    vanillaActorInit_Player.update(thisx, globalCtx);

    if (rInputCtx.cur.l && rInputCtx.cur.r) {
        start_loop();
    }
}

void PlayerActor_rDestroy(Actor* thisx, GlobalContext* globalCtx) {
    vanillaActorInit_Player.destroy(thisx, globalCtx);
}

void PlayerActor_rDraw(Actor* thisx, GlobalContext* globalCtx) {
    vanillaActorInit_Player.draw(thisx, globalCtx);
}

typedef void (*callSFX_proc)(Actor *actor, u32 sfx_id);
#define callSFX ((callSFX_proc)0x36f59c)

#define sFpsItemNoAmmoSfx ((u32*)0x53A288)
#define sFpsItemReadySfx ((u32*)0x53C000)

static u8 calledSFX = 0;
static u8 crashed = 0;

void crash(s32 r0) {
    calledSFX = 1;
    if (r0 == 0) {
        crashed = 1;
    }
}

u8 check_crashed() {
    return crashed;
}

u8 show_message(s16 stickFlameTimer, u8 calledSFX, u32 sfxId) {
    Draw_Lock();
    Draw_ClearFramebuffer();
    if (calledSFX) {
        Draw_DrawFormattedString(20, 70, COLOR_TITLE, "No crash");
        Draw_DrawFormattedString(20, 90, COLOR_WHITE, "stickFlameTimer: %08X", stickFlameTimer);
        Draw_DrawFormattedString(20, 110, COLOR_WHITE, "sfxId: %08X", sfxId);
    } else {
        Draw_DrawFormattedString(20, 70, COLOR_WHITE, "Crashy function not called");
        Draw_DrawFormattedString(20, 90, COLOR_WHITE, "stickFlameTimer: %08X", stickFlameTimer);
    }
    Draw_FlushFramebuffer();
    Draw_Unlock();

    while(1) {
        u32 pressed = Input_WaitWithTimeout(1000);

        if(pressed & BUTTON_B) {
            return 1;
        }

        if(pressed & BUTTON_A) {
            break;
        }
    }

    return 0;
}

void start_loop() {
    for (u32 stickFlameTimer = 1; stickFlameTimer < 0x50; stickFlameTimer++) {
        calledSFX = 0;
        crashed = 0;

        callSFX(&PLAYER->actor, sFpsItemNoAmmoSfx[stickFlameTimer]);

        if (!crashed) {
            u8 stop = show_message(stickFlameTimer, calledSFX, sFpsItemNoAmmoSfx[stickFlameTimer]);
            if (stop) {
                break;
            }
        }
    }

    calledSFX = 0;
    crashed = 0;
}
