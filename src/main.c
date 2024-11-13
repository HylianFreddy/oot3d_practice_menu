/*
 * This file is a distant descendent of a file originally written by n3rdswithgame
 */

#include "menu.h"
#include "menus.h"
#include "advance.h"
#include "draw.h"
#include "input.h"
#include "common.h"
#include "menus/cheats.h"
#include "menus/watches.h"
#include "menus/commands.h"
#include "menus/scene.h"
#include "menus/settings.h"
#include "3ds/extdata.h"
#include "3ds/services/irrst.h"
#include <string.h>

#include "z3D/z3D.h"
#include "actor.h"
#include "camera.h"
#include "commit_string.h"
#include "colview.h"

#define NOCLIP_SLOW_SPEED 8
#define NOCLIP_FAST_SPEED 30

advance_ctx_t advance_ctx = {};
uint8_t practice_menu_init = 0;
static bool isAsleep = false;
static u32 sAlertFrames = 0;
static char* sAlertMessage = "";
static u32 sAlertPosX;
bool menuOpen = false;

GlobalContext* gGlobalContext;
void* gStoredActorHeapAddress;
u8 gInit = 0;

void autoLoadSaveFile(void);
void NoClip_Update(void);

// Called once for every update on the `PlayState` GameState.
void before_Play_Update(GlobalContext* globalCtx) {
    if (!gInit) {
        gGlobalContext = globalCtx;
        gStoredActorHeapAddress = gActorHeapAddress;
        Actor_Init();
        irrstInit();
        Settings_UpdateWatchAddresses();
        gInit = 1;
    }
}

// Called between the `PlayState` update and draw functions.
void after_Play_Update(GlobalContext* globalCtx) {
    if (waitingButtonRelease) {
        waitingButtonRelease = (rInputCtx.cur.val != 0);
    }
    NoClip_Update();
    FreeCam_Update();
}

// Called after the `PlayState` draw function.
void after_Play_Draw() {
    ColView_DrawCollision();
}

// Called once for every update on any GameState, before all the functions in the Graph_ThreadEntry loop.
void before_GameState_Loop(GameState* gameState) {
    if (!gInit || gameState->running != 2)
        return;

    Input_Update();

    Command_UpdateCommands(rInputCtx.cur.val);
    applyCheats();
    autoLoadSaveFile();
}

static void toggle_advance(void) {
    if(pauseUnpause && advance_ctx.advance_state == NORMAL && !advance_ctx.latched){
        advance_ctx.advance_state = PAUSED;
        advance_ctx.latched = 1;
    } else if(pauseUnpause && advance_ctx.advance_state != NORMAL && !advance_ctx.latched) {
        advance_ctx.advance_state = NORMAL;
        advance_ctx.latched = 1;
    } else if (frameAdvance && advance_ctx.advance_state == NORMAL){
        advance_ctx.advance_state = LATCHED;
    } else if(!pauseUnpause) {
        advance_ctx.latched = 0;
    }
}

void setAlert(char* alertMessage, u32 alertFrames) {
    if (SETTING_ENABLED(SETTINGS_HIDE_ALERT_DISPLAY)) {
        sAlertFrames = 0;
        return;
    }

    // Clear old message if present
    Draw_DrawFormattedStringTop(sAlertPosX, 220, COLOR_WHITE, "%*s", strlen(sAlertMessage), "");

    u32 newLen = strlen(alertMessage);
    sAlertPosX = newLen <= 20 ? 280 : (SCREEN_TOP_WIDTH - newLen * SPACING_X);
    sAlertMessage = alertMessage;
    sAlertFrames = alertFrames;
}

void drawAlert(void) {
    if (sAlertFrames > 0) {
        Draw_DrawStringTop(sAlertPosX, 220, COLOR_WHITE, sAlertMessage);
        Draw_FlushFramebufferTop();
        sAlertFrames--;
    } else if (strlen(sAlertMessage) > 0) {
        setAlert("", 0);
    }
}

static void titleScreenDisplay(void) {
    Draw_DrawCenteredStringTop(14, COLOR_WHITE, "OoT3D Practice Patch");
    Draw_DrawCenteredStringTop(25, COLOR_WHITE, COMMIT_STRING);
    Draw_FlushFramebufferTop();

    char menuComboString[COMMAND_COMBO_STRING_SIZE];
    Commands_ComboToString(menuComboString, COMMAND_OPEN_MENU);
    Draw_DrawCenteredStringBottom(0, COLOR_WHITE, menuComboString);
    Draw_FlushFramebuffer();
}

void pauseDisplay(void) {
    if (SETTING_ENABLED(SETTINGS_HIDE_ALERT_DISPLAY))
        return;

    Draw_DrawFormattedStringTop(20, 20, COLOR_WHITE, "Paused");
    Draw_FlushFramebufferTop();
}

// Called from Gfx_Update, generally twice per game update cycle (but sometimes it only happens once for some reason).
void advance_main(void) {
    if(practice_menu_init == 0){
        Draw_SetupFramebuffer();
        extDataInit();
        Settings_LoadExtSaveData();
        practice_menu_init = 1;
    }

    if(shouldDrawWatches) {
        Watches_DrawWatches(COLOR_WHITE);
    }

    if(gSaveContext.entranceIndex == 0x0629 && gSaveContext.cutsceneIndex == 0xFFF3 && gSaveContext.gameMode != 2){
        titleScreenDisplay();
    }

    drawAlert();

    if (CollisionOption(COLVIEW_SHOW_COLLISION) && gColViewDisplayCountInfo && isInGame()) {
        s16 polyMax = gMainClass->sub32A0.polyMax;
        s16 polyCounter = gMainClass->sub32A0.polyCounter;
        Draw_DrawFormattedString(3, SCREEN_BOT_HEIGHT - SPACING_Y, polyCounter >= polyMax ? COLOR_RED : COLOR_WHITE,
                                 "%d/%d", polyCounter, polyMax);
    }

    if(menuOpen) {
        menuShow(&gz3DMenu);

        Draw_Lock();
        Draw_ClearFramebuffer();
        Draw_FlushFramebuffer();
        Draw_Unlock();
        rInputCtx.cur.val = 0;
    }

    toggle_advance();

    if(advance_ctx.advance_state == STEP) {
        if(frameAdvance) {
            advance_ctx.advance_state = LATCHED;
        } else {
            advance_ctx.advance_state = PAUSED;
        }
    }
    pauseUnpause = 0;
    frameAdvance = 0;

    while(!isAsleep &&(advance_ctx.advance_state == PAUSED || advance_ctx.advance_state == LATCHED)) {
        pauseDisplay();
        Input_Update();
        Command_UpdateCommands(rInputCtx.cur.val);
        drawAlert();
        if(menuOpen) {
            menuShow(&gz3DMenu);
            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            Draw_Unlock();
        }
        applyCheats();
        toggle_advance();
        if(advance_ctx.advance_state == LATCHED && !frameAdvance) {
            advance_ctx.advance_state = PAUSED;
        }
        if(advance_ctx.advance_state == PAUSED && frameAdvance) {
            advance_ctx.advance_state = STEP;
        }
        pauseUnpause = 0;
        frameAdvance = 0;
        svcSleepThread(16E6);
        rInputCtx.cur.val = 0;
    }
    isAsleep = false;
}

void NoClip_Update(void) {
    if (!noClip || waitingButtonRelease) {
        return;
    }
    if (!isInGame()) {
        // Quit NoClip when loading a new scene
        noClip = FALSE;
        haltActors = FALSE;
        return;
    }

    u32 in = rInputCtx.cur.val;
    f32 amount = (in & BUTTON_X) ? NOCLIP_FAST_SPEED : NOCLIP_SLOW_SPEED;
    if(in & BUTTON_L1) {
        PLAYER->actor.world.pos.y += amount;
    }
    if(in & BUTTON_R1) {
        PLAYER->actor.world.pos.y -= amount;
    }
    if(in & (BUTTON_DOWN)) {
        PLAYER->actor.world.pos.z += amount;
    }
    if(in & (BUTTON_UP)) {
        PLAYER->actor.world.pos.z -= amount;
    }
    if(in & (gSaveContext.masterQuestFlag ? BUTTON_RIGHT : BUTTON_LEFT)) {
        PLAYER->actor.world.pos.x -= amount;
    }
    if(in & (gSaveContext.masterQuestFlag ? BUTTON_LEFT : BUTTON_RIGHT)) {
        PLAYER->actor.world.pos.x += amount;
    }

    s16 yaw = gGlobalContext->cameraPtrs[gGlobalContext->activeCamera]->camDir.y;
    if(ControlStick_X * ControlStick_X + ControlStick_Y * ControlStick_Y > 100) {
        PLAYER->actor.world.pos.x += 0.02 * amount * (ControlStick_Y * sins(yaw) - ControlStick_X * coss(yaw));
        PLAYER->actor.world.pos.z += 0.02 * amount * (ControlStick_Y * coss(yaw) + ControlStick_X * sins(yaw));
    }

    if(rInputCtx.pressed.val & BUTTON_Y) {
        haltActors = !haltActors;
    }

    if(in & BUTTON_A) { //confirm new position
        PLAYER->actor.home.pos = PLAYER->actor.world.pos;
        Scene_NoClipToggle();
        Commands_SetButtonsToIgnore(BUTTON_A);
    }
    else if(in & BUTTON_B) { //cancel movement
        PLAYER->actor.world.pos = PLAYER->actor.home.pos;
        Scene_NoClipToggle();
        Commands_SetButtonsToIgnore(BUTTON_B);
    }
}

void Gfx_SleepQueryCallback(void) {
    menuOpen = false;
    isAsleep = true;
}

bool onMenuLoop(void) {
    sAlertFrames -= MIN(30, sAlertFrames);
    drawAlert();
    return menuOpen;
};

void autoLoadSaveFile(void) {
    if (gSaveContext.entranceIndex == 0x629 && gSaveContext.cutsceneIndex == 0xFFF3 && shouldAutoloadSavefile) {
        Load_Savefiles_Buffer();
        FileSelect_LoadGame(&gGlobalContext->state, 0);
        if (gSaveContext.saveCount > 0) {
            setAlert("Autoload File 1", 90);
            gGlobalContext->linkAgeOnLoad = gSaveContext.linkAge;
#if !Version_KOR && !Version_TWN
            if (gSaveContext.masterQuestFlag) {
                // These static variables are used at some point during the load to overwrite the MQ flag.
                // Setting them like this is kind of broken (saving the game will save onto MQ slot 1),
                // but the autoloaded file shouldn't be MQ anyway.
                *(u8*)0x587934 = 0xBE; // Enable quest type buttons on title screen
                *(u8*)0x587953 = 0xEF; // Pressed the MQ button
            }
#endif
        } else {
            setAlert("File 1 is empty", 90);
        }
    }
}

// Called for every update cycle in Graph_ThreadEntry
// Returning true will skip drawing the frame on screen, making the game speed-up significantly
// (exactly how much depends on the CPU speed).
s32 checkFastForward(void) {
    if (shouldFastForward) {
        gFastForwardCycleCounter = (gFastForwardCycleCounter + 1) % FAST_FORWARD_CYCLE_MAX;
        return gFastForwardCycleCounter != 0;
    }
    return FALSE;
}
