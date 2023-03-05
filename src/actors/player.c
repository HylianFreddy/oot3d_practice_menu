#include "z3D/z3D.h"
#include "actors/player.h"
#include "draw.h"
#include "input.h"

ActorInit vanillaActorInit_Player = {0};

#define LOOP_MAX 0x7FFF
#define LOOP_START 0x1
#define SFX_ARRAY sFpsItemReadySfx

#define STOP_AND_PLAY_SFX 1
#define RESET_LOOP 2

typedef void (*callSFX_proc)(Actor *actor, u32 sfx_id);
#define callSFX ((callSFX_proc)0x36f59c)

#define sFpsItemNoAmmoSfx ((u32*)0x53A288)
#define sFpsItemReadySfx ((u32*)0x53C000)

void advance_sfx_test();
void next_sfx();
u8 show_message(s16 stickFlameTimer, u8 calledSFX, u32 sfxId);
static u8 calledSFX = 0;
static u8 lastCalledSFX = 0;
static u8 crashed = 0;
static u8 continueLoop = 0;
static u8 loopStatus = 0;
static u32 loopCounter = LOOP_START;
static u32 currentSfxStickTimer = 0;
static u32 currentSfxId = 0;

void PlayerActor_rInit(Actor* thisx, GlobalContext* globalCtx) {
    vanillaActorInit_Player.init(thisx, globalCtx);
}

void PlayerActor_rUpdate(Actor* thisx, GlobalContext* globalCtx) {
    vanillaActorInit_Player.update(thisx, globalCtx);

    advance_sfx_test();
}

void PlayerActor_rDestroy(Actor* thisx, GlobalContext* globalCtx) {
    vanillaActorInit_Player.destroy(thisx, globalCtx);
}

void PlayerActor_rDraw(Actor* thisx, GlobalContext* globalCtx) {
    vanillaActorInit_Player.draw(thisx, globalCtx);
}

void advance_sfx_test() {
    if (continueLoop && !crashed) {
        loopStatus = show_message(currentSfxStickTimer, lastCalledSFX, currentSfxId);
    } else if (gSaveContext.rupees == 0 && rInputCtx.cur.l && rInputCtx.cur.r) {
        show_message(0, 0, 0);
        continueLoop = 1;
        loopStatus = 0;
    }

    if (continueLoop) {
        next_sfx();
        if (loopStatus == RESET_LOOP || loopCounter >= LOOP_MAX) {
            loopCounter = LOOP_START;
            continueLoop = 0;
        } else {
            loopCounter++;
            continueLoop = 1;
        }
    } else {
        calledSFX = 0;
        crashed = 0;
    }
}

void next_sfx() {
    for (; loopCounter < LOOP_MAX; loopCounter++) {
        calledSFX = 0;
        crashed = 0;

        currentSfxStickTimer = loopCounter;
        currentSfxId = SFX_ARRAY[loopCounter];

        callSFX(&PLAYER->actor, currentSfxId);
        lastCalledSFX = calledSFX;

        if (!crashed) {
            break;
        }
    }
}

u8 show_message(s16 stickFlameTimer, u8 calledSFX, u32 sfxId) {
    static u32 prevSfxId[6] = {1,2,3,4,5,6};
    Draw_Lock();
    Draw_ClearFramebuffer();
    if (calledSFX) {
        Draw_DrawFormattedString(20, 70, COLOR_TITLE, "No crash");
        Draw_DrawFormattedString(20, 90, COLOR_WHITE, "stickFlameTimer: 0x%04X (%01d)", stickFlameTimer, stickFlameTimer);
        Draw_DrawFormattedString(20, 110, COLOR_WHITE, "sfxId: %08X", sfxId);
    } else {
        Draw_DrawFormattedString(20, 70, COLOR_WHITE, "Crashy function not called");
        Draw_DrawFormattedString(20, 90, COLOR_WHITE, "stickFlameTimer: %08X", stickFlameTimer);
    }

    if (stickFlameTimer == 0) {
        Draw_ClearFramebuffer();
    }
    Draw_FlushFramebuffer();
    Draw_Unlock();

    while(1) {
        u32 timeout;
        if (sfxId != prevSfxId[0]) {
            timeout = 1000;
        } else if (sfxId != prevSfxId[1]) {
            timeout = 1000;
        } else if (sfxId != prevSfxId[2]) {
            timeout = 500;
        } else if (sfxId != prevSfxId[3]) {
            timeout = 500;
        } else if (sfxId != prevSfxId[4]) {
            timeout = 500;
        } else if (sfxId != prevSfxId[5]) {
            timeout = 300;
        } else {
            timeout = 300;
        }
        prevSfxId[5] = prevSfxId[4];
        prevSfxId[4] = prevSfxId[3];
        prevSfxId[3] = prevSfxId[2];
        prevSfxId[2] = prevSfxId[1];
        prevSfxId[1] = prevSfxId[0];
        prevSfxId[0] = sfxId;

        u32 pressed = Input_WaitWithTimeout(timeout);

        if(pressed & BUTTON_R1) {
            return RESET_LOOP;
        }

        return STOP_AND_PLAY_SFX;
    }

    return 0;
}

void crash(s32 r0) {
    calledSFX = 1;
    if (r0 == 0) {
        crashed = 1;
    }
}

u8 check_crashed() {
    return crashed;
}
