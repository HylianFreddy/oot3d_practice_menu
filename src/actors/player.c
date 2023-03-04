#include "z3D/z3D.h"
#include "actors/player.h"
#include "draw.h"
#include "input.h"

ActorInit vanillaActorInit_Player = {0};
void start_loop();
static u8 continueLoop = 0;

void PlayerActor_rInit(Actor* thisx, GlobalContext* globalCtx) {
    vanillaActorInit_Player.init(thisx, globalCtx);
}

void PlayerActor_rUpdate(Actor* thisx, GlobalContext* globalCtx) {
    vanillaActorInit_Player.update(thisx, globalCtx);

    if (continueLoop || (gSaveContext.rupees == 0 && rInputCtx.cur.l && rInputCtx.cur.r)) {
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

        if(pressed & BUTTON_R1) {
            return 2;
        }

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
    static u32 loopCounter = 0x1;
    static u8 stop = 0;

    for (; loopCounter < 0x7FFF; loopCounter++) {
        calledSFX = 0;
        crashed = 0;

        callSFX(&PLAYER->actor, sFpsItemReadySfx[loopCounter]);

        if (!crashed) {
            stop = show_message(loopCounter, calledSFX, sFpsItemReadySfx[loopCounter]);
            if (stop) {
                break;
            }
        }
    }

    calledSFX = 0;
    crashed = 0;

    if (stop == 2) {
        loopCounter = 0x1;
        continueLoop = 0;
    } else if (loopCounter >= 0x7FFF) {
        loopCounter = 1;
        continueLoop = 0;
    } else {
        loopCounter++;
        continueLoop = 1;
    }
}
