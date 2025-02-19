#include "menu.h"
#include "menus/commands.h"
#include "menus/warps.h"
#include "menus/watches.h"
#include "menus/scene.h"
#include "menus/settings.h"
#include "input.h"
#include "common.h"
#include "z3D/z3D.h"
#include "draw.h"
#include "advance.h"
#include "camera.h"
#include "colview.h"

#include <string.h>

u32 pauseUnpause = 0; //tells main to pause/unpause
u32 frameAdvance = 0; //tells main to frame advance
bool shouldDrawWatches = 1;
u32 shouldAutoloadSavefile = 0;
u32 shouldFastForward = 0;
u32 gFastForwardCycleCounter = 0;

PosRot storedPosRot[STORED_POS_COUNT];
static u8 storedPosIndex = 0;

// Used to make the game ignore button presses that were used for specific commands.
btn_t buttonsToIgnore = { 0 };

// Button names in the same order as the `BUTTON_` values; 4 characters each
static char buttonNames[15][COMMAND_BUTTON_NAME_LENGTH] = {
    "A   ",    // BUTTON_A
    "B   ",    // BUTTON_B
    "Sel ",    // BUTTON_SELECT
    "St  ",    // BUTTON_START
    "\x1A   ", // BUTTON_RIGHT
    "\x1B   ", // BUTTON_LEFT
    "\x18   ", // BUTTON_UP
    "\x19   ", // BUTTON_DOWN
    "R   ",    // BUTTON_R1
    "L   ",    // BUTTON_L1
    "X   ",    // BUTTON_X
    "Y   ",    // BUTTON_Y
    "    ",    // Unused
    "ZL  ",    // BUTTON_ZL
    "ZR  ",    // BUTTON_ZR
};

static void Command_OpenMenu(void){
    menuOpen = true;
}

static void Command_Break(void){
    if (isInGame()) {
        if (gGlobalContext->csCtx.state != CS_STATE_IDLE) {
            gGlobalContext->csCtx.state = CS_STATE_STOP;
        }
        if (gGlobalContext->subCameras[0].timer != -1) {
            gGlobalContext->subCameras[0].timer = 0;
        }

        // "aggressive" break
#if !Version_KOR && !Version_TWN
        Message_CloseTextbox(gGlobalContext);
#endif
        gGlobalContext->mainCamera.setting   = 1;
        gGlobalContext->mainCamera.animState = 0;
        PLAYER->stateFlags1 = 0x0;
        PLAYER->stateFlags2 = 0x0;
        if (PLAYER->csAction != 0) {
            PLAYER->csAction = 7;
        }

        setAlert("Break", 40);
    }
}

static void Command_Levitate(void){
    if (isInGame()) PLAYER->actor.velocity.y = 6.34375f;
}

static void Command_Fall(void){
    if (isInGame()){
        PLAYER->actor.home.pos.y = -4096.f;
    }
}

static void Command_RunFast(void){
    if (isInGame()) PLAYER->xzSpeed = 27.f;
}

static void Command_Reset(void){
    if (isInGame()){
        gSaveContext.respawnFlag = 0;
        EntranceWarp(0xFFFF, 0, -1, 0, FALSE);
    }
}

static void Command_ReloadScene(void){
    if (isInGame()) {
        if(gGlobalContext->nextEntranceIndex != -1)
            EntranceWarp(gGlobalContext->nextEntranceIndex, gGlobalContext->linkAgeOnLoad, -1, 0, FALSE);
        else
            EntranceWarp(gSaveContext.entranceIndex, gGlobalContext->linkAgeOnLoad, -1, 0, FALSE);
    }
}

static void Command_VoidOut(void){
    if (isInGame()) {
        gSaveContext.respawn[RESPAWN_MODE_DOWN].tempSwchFlags = gGlobalContext->actorCtx.flags.tempSwch;
        gSaveContext.respawn[RESPAWN_MODE_DOWN].tempCollectFlags = gGlobalContext->actorCtx.flags.tempCollect;
        gSaveContext.respawnFlag = 1;
        if (gGlobalContext->transitionTrigger == 0 || gGlobalContext->transitionTrigger == -20) {
            EntranceWarp(gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex, gGlobalContext->linkAgeOnLoad, -1, 0, FALSE);
        }
    }
}

static void Command_ToggleAge(void){
    gGlobalContext->linkAgeOnLoad = 1 - gGlobalContext->linkAgeOnLoad;
    setAlert(gGlobalContext->linkAgeOnLoad ? "Child on next load" : "Adult on next load", 75);
}

// static void Command_SaveState(void);
// static void Command_LoadState(void);

static void Command_StorePos(void){
    if (isInGame()){
        storedPosRot[storedPosIndex].pos = PLAYER->actor.world.pos;
        storedPosRot[storedPosIndex].rot = PLAYER->actor.world.rot;

        char* alert = "Stored position X";
        alert[16] = storedPosIndex + '0';
        setAlert(alert, 90);
    }
}

static void Command_LoadPos(void){
    if (isInGame()){
        PLAYER->actor.home.pos = storedPosRot[storedPosIndex].pos;
        PLAYER->actor.world.pos = storedPosRot[storedPosIndex].pos;
        PLAYER->actor.world.rot = storedPosRot[storedPosIndex].rot;
        PLAYER->actor.focus.rot = storedPosRot[storedPosIndex].rot;
        PLAYER->actor.shape.rot = storedPosRot[storedPosIndex].rot;

        char* alert = "Loaded position X";
        alert[16] = storedPosIndex + '0';
        setAlert(alert, 90);
    }
}

static void AlertPosIndex(void) {
    char* alert = "Position X";
    alert[9] = storedPosIndex + '0';
    setAlert(alert, 75);
}

static void Command_PreviousPos(void) {
    storedPosIndex = (storedPosIndex+STORED_POS_COUNT-1) % STORED_POS_COUNT;
    AlertPosIndex();
}
static void Command_NextPos(void) {
    storedPosIndex = (storedPosIndex+1) % STORED_POS_COUNT;
    AlertPosIndex();
}

static void Command_PauseUnpause(void){
    pauseUnpause = 1;
    if (advance_ctx.advance_state == NORMAL) {
        pauseDisplay(); // appear when triggered from menu
    }
}

static void Command_FrameAdvance(void){
    frameAdvance = 1;
    if (advance_ctx.advance_state == NORMAL) {
        pauseDisplay(); // appear when triggered from menu
    }
}

// static void Command_RecordMacro(void);
// static void Command_PlayMacro(void);

static void Command_CollisionView(void) {
    CollisionMenu.items[COLVIEW_SHOW_COLLISION].on ^= 1;
}

static void Command_HitboxView(void){
    gStaticContext.showColliders ^= 1;
}

static void Command_ToggleWatches(void){
    shouldDrawWatches = !shouldDrawWatches;
    if (advance_ctx.advance_state == PAUSED) {
        Watches_DrawWatches(shouldDrawWatches ? COLOR_WHITE : COLOR_BLACK);
    }
}

static void Command_FreeCam(void){
    if (!freeCam.enabled) {
        FreeCam_Toggle();
    } else if (freeCam.locked) {
        FreeCam_ToggleLock();
    } else {
        return;
    }

    if (advance_ctx.advance_state == PAUSED) {
        // Unpause automatically when entering FreeCam
        pauseUnpause = 1;
    }

    waitingButtonRelease = 1;
}

static void Command_TriggerSavefileAutoload(void){
    shouldAutoloadSavefile = 1;
}

static void Command_TriggerFastForward(void){
    shouldFastForward = 1;
}

Command commandList[NUMBER_OF_COMMANDS] = {
    {"Open Menu", 0, 0, { 0 }, Command_OpenMenu, COMMAND_PRESS_ONCE_TYPE, 0, 0},
    {"Levitate", 0, 0, { 0 }, Command_Levitate, COMMAND_HOLD_TYPE, 0, 0},
    {"Fall", 0, 0, { 0 }, Command_Fall, COMMAND_HOLD_TYPE, 0, 0},
    {"Run Fast", 0, 0, { 0 }, Command_RunFast, COMMAND_HOLD_TYPE, 0, 0},
    {"Reset", 0, 0, { 0 }, Command_Reset, COMMAND_PRESS_ONCE_TYPE, 0, 0},
    {"Reload Scene", 0, 0, { 0 }, Command_ReloadScene, COMMAND_PRESS_ONCE_TYPE, 0, 0},
    {"Void Out", 0, 0, { 0 }, Command_VoidOut, COMMAND_PRESS_ONCE_TYPE, 0, 0},
    {"Toggle Age", 0, 0, { 0 }, Command_ToggleAge, COMMAND_PRESS_TYPE, 0, 0},
    {"Store Position", 0, 0, { 0 }, Command_StorePos, COMMAND_HOLD_TYPE, 0, 0},
    {"Load Position", 0, 0, { 0 }, Command_LoadPos, COMMAND_HOLD_TYPE, 0, 0},
    {"Previous Position", 0, 0, { 0 }, Command_PreviousPos, COMMAND_PRESS_TYPE, 0, 0},
    {"Next Position", 0, 0, { 0 }, Command_NextPos, COMMAND_PRESS_TYPE, 0, 0},
    {"Pause/Unpause", 0, 0, { 0 }, Command_PauseUnpause, COMMAND_PRESS_TYPE, 0, 0},
    {"Frame Advance", 0, 0, { 0 }, Command_FrameAdvance, COMMAND_PRESS_TYPE, 0, 0},
    {"Hitbox View", 0, 0, { 0 }, Command_HitboxView, COMMAND_PRESS_TYPE, 0, 0},
    {"Collision View", 0, 0, { 0 }, Command_CollisionView, COMMAND_PRESS_TYPE, 0, 0},
    {"Toggle Watches", 0, 0, { 0 }, Command_ToggleWatches, COMMAND_PRESS_TYPE, 0, 0},
    {"Break Free", 0, 0, { 0 }, Command_Break, COMMAND_HOLD_TYPE, 0, 0},
    {"NoClip", 0, 0, { 0 }, Scene_NoClipToggle, COMMAND_PRESS_ONCE_TYPE, 0, 0},
    {"Free Camera", 0, 0, { 0 }, Command_FreeCam, COMMAND_PRESS_ONCE_TYPE, 0, 0},
    {"Autoload Savefile", 0, 0, { 0 }, Command_TriggerSavefileAutoload, COMMAND_HOLD_TYPE, 0, 0},
    {"Fast Forward", 0, 0, { 0 }, Command_TriggerFastForward, COMMAND_HOLD_TYPE, 0, 0},
};

static void Commands_ListInitDefaults(void){
    // reset all commands for when default settings are restored
    for(u32 i = 0; i < NUMBER_OF_COMMANDS; ++i) {
        commandList[i].comboLen = 0;
        commandList[i].strict = 0;
    }

    // storing all previous buttons for every index is needed to make the strict commands work
    commandList[COMMAND_OPEN_MENU].comboLen = 3;
    commandList[COMMAND_OPEN_MENU].inputs[0] = BUTTON_L1;
    commandList[COMMAND_OPEN_MENU].inputs[1] = BUTTON_R1 | BUTTON_L1;
    commandList[COMMAND_OPEN_MENU].inputs[2] = BUTTON_SELECT | BUTTON_R1 | BUTTON_L1;
    commandList[COMMAND_OPEN_MENU].strict = 0;

    commandList[COMMAND_LEVITATE].comboLen = 2;
    commandList[COMMAND_LEVITATE].inputs[0] = BUTTON_R1;
    commandList[COMMAND_LEVITATE].inputs[1] = BUTTON_X | BUTTON_R1;
    commandList[COMMAND_LEVITATE].strict = 0;

    commandList[COMMAND_FALL].comboLen = 2;
    commandList[COMMAND_FALL].inputs[0] = BUTTON_R1;
    commandList[COMMAND_FALL].inputs[1] = BUTTON_RIGHT | BUTTON_R1;
    commandList[COMMAND_FALL].strict = 0;

    commandList[COMMAND_RUN_FAST].comboLen = 2;
    commandList[COMMAND_RUN_FAST].inputs[0] = BUTTON_R1;
    commandList[COMMAND_RUN_FAST].inputs[1] = BUTTON_Y | BUTTON_R1;
    commandList[COMMAND_RUN_FAST].strict = 0;

    commandList[COMMAND_VOID_OUT].comboLen = 3;
    commandList[COMMAND_VOID_OUT].inputs[0] = BUTTON_A;
    commandList[COMMAND_VOID_OUT].inputs[1] = BUTTON_B | BUTTON_A;
    commandList[COMMAND_VOID_OUT].inputs[2] = BUTTON_SELECT | BUTTON_B | BUTTON_A;
    commandList[COMMAND_VOID_OUT].strict = 0;

    commandList[COMMAND_STORE_POS].comboLen = 2;
    commandList[COMMAND_STORE_POS].inputs[0] = BUTTON_L1;
    commandList[COMMAND_STORE_POS].inputs[1] = BUTTON_LEFT | BUTTON_L1;
    commandList[COMMAND_STORE_POS].strict = 0;

    commandList[COMMAND_LOAD_POS].comboLen = 2;
    commandList[COMMAND_LOAD_POS].inputs[0] = BUTTON_L1;
    commandList[COMMAND_LOAD_POS].inputs[1] = BUTTON_RIGHT | BUTTON_L1;
    commandList[COMMAND_LOAD_POS].strict = 0;

    commandList[COMMAND_PREVIOUS_POS].comboLen = 1;
    commandList[COMMAND_PREVIOUS_POS].inputs[0] = BUTTON_LEFT;
    commandList[COMMAND_PREVIOUS_POS].strict = 1;

    commandList[COMMAND_NEXT_POS].comboLen = 1;
    commandList[COMMAND_NEXT_POS].inputs[0] = BUTTON_RIGHT;
    commandList[COMMAND_NEXT_POS].strict = 1;

    commandList[COMMAND_TOGGLE_PAUSE].comboLen = 1;
    commandList[COMMAND_TOGGLE_PAUSE].inputs[0] = BUTTON_DOWN;
    commandList[COMMAND_TOGGLE_PAUSE].strict = 0;

    commandList[COMMAND_FRAME_ADVANCE].comboLen = 1;
    commandList[COMMAND_FRAME_ADVANCE].inputs[0] = BUTTON_UP;
    commandList[COMMAND_FRAME_ADVANCE].strict = 0;

    commandList[COMMAND_BREAK].comboLen = 4;
    commandList[COMMAND_BREAK].inputs[0] = BUTTON_A;
    commandList[COMMAND_BREAK].inputs[1] = BUTTON_B | BUTTON_A;
    commandList[COMMAND_BREAK].inputs[2] = BUTTON_X | BUTTON_B | BUTTON_A;
    commandList[COMMAND_BREAK].inputs[3] = BUTTON_Y | BUTTON_X | BUTTON_B | BUTTON_A;
    commandList[COMMAND_BREAK].strict = 0;

    commandList[COMMAND_FAST_FORWARD].comboLen = 1;
    commandList[COMMAND_FAST_FORWARD].inputs[0] = BUTTON_ZL;
    commandList[COMMAND_FAST_FORWARD].strict = 0;
}

u32 commandInit = 0;
void Command_UpdateCommands(u32 curInputs){ //curInputs should be all the held and pressed buttons
    if (!commandInit){
        Commands_ListInitDefaults();
        commandInit = 1;
    }

    // Reset these every time
    shouldAutoloadSavefile = 0;
    shouldFastForward = 0;

    if (commandList[COMMAND_OPEN_MENU].comboLen == 0) { // prevent getting locked out of the menu
        commandList[COMMAND_OPEN_MENU].comboLen = 3;
        commandList[COMMAND_OPEN_MENU].inputs[0] = BUTTON_L1;
        commandList[COMMAND_OPEN_MENU].inputs[1] = BUTTON_R1;
        commandList[COMMAND_OPEN_MENU].inputs[2] = BUTTON_SELECT;
        commandList[COMMAND_OPEN_MENU].strict = 0;
    }

    u8 executedPressCommand = 0;

    for (int i = 0; i < NUMBER_OF_COMMANDS; i++) {
        Command* cmd = &commandList[i];

        if (cmd->comboLen == 0 ||
            (i == COMMAND_AUTOLOAD_SAVEFILE && (gSaveContext.entranceIndex != 0x629 || gSaveContext.cutsceneIndex != 0xFFF3))
        ) {
            continue;
        }

        u32 nextInputs     = cmd->inputs[cmd->curIdx];
        u32 previousInputs = (cmd->curIdx > 0) ? cmd->inputs[cmd->curIdx - 1] : 0;

        if ((cmd->strict && curInputs == nextInputs) ||
            (!cmd->strict && (curInputs & nextInputs) == nextInputs)) { // case where we hit the new button

            if (cmd->curIdx == cmd->comboLen - 1) { // time to execute the command
                switch (cmd->type) {
                    case COMMAND_HOLD_TYPE:
                        cmd->method();
                        break;
                    case COMMAND_PRESS_ONCE_TYPE:
                        cmd->curIdx = 0;
                        // fallthrough
                    case COMMAND_PRESS_TYPE:
                        // execute at most one "press" or "press_once" command per update cycle
                        if (cmd->waiting == 0 && !executedPressCommand) {
                            cmd->method();
                            cmd->waiting = 1;
                            executedPressCommand = 1;
                        }
                        break;
                }
                u32 lastButton = cmd->inputs[cmd->comboLen - 1] &
                                ~(cmd->comboLen > 1 ? cmd->inputs[cmd->comboLen - 2] : 0);
                Commands_SetButtonsToIgnore(lastButton);
            } else {
                cmd->curIdx++;
            }
        } else if (cmd->curIdx > 0 &&
                   ((cmd->strict && curInputs == previousInputs) ||
                    (!cmd->strict && (curInputs & previousInputs) == previousInputs))) { // case where inputs still held

            cmd->waiting = 0;
        } else { // case where command resets
            cmd->curIdx  = 0;
            cmd->waiting = 0;
        }

        if (noClip || FreeCam_Moving)
            break; // only the "open menu" command should work during noclip mode
    }
}

void Commands_ComboToString(char buf[COMMAND_COMBO_STRING_SIZE], u32 commandIdx){
    u32 prevInput = 0;
    u32 charIdx = 0;

    if (commandList[commandIdx].comboLen == 0) {
        // Set combo string to whitespaces to overwrite previously drawn characters
        memset(buf, ' ', COMMAND_COMBO_STRING_SIZE);
        buf[COMMAND_COMBO_STRING_SIZE - 1] = '\x00';
        return;
    }

    for (u32 i = 0; i < commandList[commandIdx].comboLen; ++i){
        u32 newInput = commandList[commandIdx].inputs[i] & ~prevInput;
        u32 btnIdx = __builtin_ctz(newInput);
        memcpy(buf + charIdx, buttonNames[btnIdx], COMMAND_BUTTON_NAME_LENGTH);
        charIdx += COMMAND_BUTTON_NAME_LENGTH;
        prevInput = commandList[commandIdx].inputs[i];
    }

    buf[charIdx - 1] = '\x00';
}

static void Commands_EditCommand(u32 commandIndex){
    u32 selected = 0;
    u32 editing = 0;
    u32 curColor = COLOR_WHITE;
    char comboString[COMMAND_COMBO_STRING_SIZE];
    u32 prevInput[COMMAND_COMBO_MAX] = {0};

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();

    do{

        Draw_Lock();
        Draw_DrawFormattedString(10, 10, COLOR_TITLE, "Edit Command: %s", commandList[commandIndex].title);

        Commands_ComboToString(comboString, commandIndex);
        Draw_DrawFormattedString(30, 30, curColor, "Combo: %s", comboString);
        Draw_DrawCharacter(10, 30, COLOR_TITLE, selected == COMMAND_EDIT_COMBO ? '>' : ' ');

        Draw_DrawFormattedString(30, 30 + SPACING_Y, COLOR_WHITE, "Type: %s", commandList[commandIndex].strict ? "Strict " : "Relaxed");
        Draw_DrawCharacter(10, 30 + SPACING_Y, COLOR_TITLE, selected == COMMAND_EDIT_STRICT ? '>' : ' ');

        Draw_DrawString(10, SCREEN_BOT_HEIGHT - 20, COLOR_TITLE, "Use X to clear the command");

        Draw_FlushFramebuffer();
        Draw_Unlock();

        if (!editing){
            Draw_Lock();
            Draw_DrawCharacter(300, 10, COLOR_WHITE, ' ');
            Draw_FlushFramebuffer();
            Draw_Unlock();
            for(u8 i = 0; i < COMMAND_COMBO_MAX; i++) {
                prevInput[i] = 0;
            }
            u32 pressed = Input_WaitWithTimeout(1000);
            if (pressed & BUTTON_A){
                if (selected == COMMAND_EDIT_STRICT){
                    commandList[commandIndex].strict = !commandList[commandIndex].strict;
                    commandList[commandIndex].curIdx = 0;
                    commandList[commandIndex].waiting = 0;
                }
                else if (selected == COMMAND_EDIT_COMBO){
                    editing = 1;
                    curColor = COLOR_RED;
                    commandList[commandIndex].comboLen = 0;
                }
            }
            else if (pressed & BUTTON_X){
                commandList[commandIndex].comboLen = 0;
            }
            else if (pressed & BUTTON_B){
                break;
            }
            else if (pressed & (PAD_DOWN | PAD_UP)){
                selected = (selected + 1) % 2;
            }
        }
        else {
            for (u32 secs = COMMAND_COMBO_TIMEOUT; secs > 0; --secs){
                Draw_Lock();
                Draw_DrawFormattedString(300, 10, COLOR_RED, "%u", secs);
                Draw_FlushFramebuffer();
                Draw_Unlock();

                u32 pressed = Input_WaitWithTimeout(1000);
                if (!pressed){
                    editing = (secs > 1);
                    curColor = (editing ? COLOR_RED : COLOR_WHITE);
                    continue;
                }

                // ignore circle pad inputs
                if (pressed & CPAD_ANY) {
                    break;
                }

                // buttons have to be pressed one at a time, ignore multiple button inputs
                u32 pressedCopy = pressed;
                u8 bitCount = 0;
                for (bitCount = 0; pressedCopy; pressedCopy >>= 1){
                    bitCount += pressedCopy & 1;
                }
                if(bitCount > 1) break;
                // when receiving a repeated input, ignore it and save the combo
                for(u8 i = 0; i < COMMAND_COMBO_MAX; i++) {
                    if(pressed == prevInput[i]){
                        editing = 0;
                        curColor = COLOR_WHITE;
                    }
                }
                if(!editing) break;

                prevInput[commandList[commandIndex].comboLen] = pressed;


                if(commandList[commandIndex].comboLen > 0){
                    commandList[commandIndex].inputs[commandList[commandIndex].comboLen] = pressed | commandList[commandIndex].inputs[commandList[commandIndex].comboLen - 1];
                }
                else {
                    commandList[commandIndex].inputs[0] = pressed;
                }
                commandList[commandIndex].comboLen++;

                if (commandList[commandIndex].comboLen >= COMMAND_COMBO_MAX){
                    editing = 0;
                    curColor = COLOR_WHITE;
                }
                break;
            }
        }

    } while(onMenuLoop());
    Draw_ClearFramebuffer();
}

void Commands_ShowCommandsMenu(void){
    static s32 selected = 0, page = 0, pagePrev = 0;

    if (SETTING_ENABLED(SETTINGS_RESET_CURSOR)) {
        selected = 0, page = 0, pagePrev = 0;
    }

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();

    do
    {
        Draw_Lock();
        if (page != pagePrev)
        {
            Draw_ClearFramebuffer();
        }
        Draw_DrawFormattedString(10, 10, COLOR_TITLE, "Commands. Press START to restore defaults");

        for (s32 i = 0; i < COMMAND_MENU_MAX_SHOW && page * COMMAND_MENU_MAX_SHOW + i < NUMBER_OF_COMMANDS; ++i)
        {
            char comboString[COMMAND_COMBO_STRING_SIZE];
            s32 j = page * COMMAND_MENU_MAX_SHOW + i;
            Commands_ComboToString(comboString, j);
            Draw_DrawFormattedString(30,  30 + i * SPACING_Y, COLOR_WHITE, "%s: ", commandList[j].title);
            Draw_DrawFormattedString(160, 30 + i * SPACING_Y, COLOR_WHITE, "%s",   comboString);
            Draw_DrawString(260, 30 + i * SPACING_Y, COLOR_WHITE, commandList[j].strict ? "Strict " : "Relaxed");
            Draw_DrawCharacter(10, 30 + i * SPACING_Y, COLOR_TITLE, j == selected ? '>' : ' ');
        }

        Draw_DrawString(10, SCREEN_BOT_HEIGHT - 20, COLOR_TITLE, "Use Y to execute the commands from here");

        Draw_FlushFramebuffer();
        Draw_Unlock();

        u32 pressed = Input_WaitWithTimeout(1000);
        if(pressed & BUTTON_B)
            break;
        if(pressed & BUTTON_A)
        {
            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            Draw_Unlock();

            Commands_EditCommand(selected);

            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            Draw_Unlock();
        }
        else if (pressed & BUTTON_L1)
        {
            selected = 0;
        }
        else if(pressed & BUTTON_Y)
        {
            commandList[selected].method();
        }
        else if(pressed & PAD_DOWN)
        {
            selected++;
        }
        else if(pressed & PAD_UP)
        {
            selected--;
        }
        else if(pressed & PAD_LEFT){
            selected -= COMMAND_MENU_MAX_SHOW;
        }
        else if(pressed & PAD_RIGHT){
            if(selected + COMMAND_MENU_MAX_SHOW < NUMBER_OF_COMMANDS)
                selected += COMMAND_MENU_MAX_SHOW;
            else if((NUMBER_OF_COMMANDS - 1) / COMMAND_MENU_MAX_SHOW == page)
                selected %= COMMAND_MENU_MAX_SHOW;
            else selected = NUMBER_OF_COMMANDS - 1;
        }
        else if(pressed & BUTTON_START)
        {
            Commands_ListInitDefaults();
        }

        if(selected < 0)
            selected = NUMBER_OF_COMMANDS - 1;
        else if(selected >= NUMBER_OF_COMMANDS) selected = 0;

        pagePrev = page;
        page = selected / COMMAND_MENU_MAX_SHOW;
    } while(onMenuLoop());
}

void Commands_SetButtonsToIgnore(u32 buttons) {
    buttonsToIgnore.val |= buttons;
    if (buttonsToIgnore.sel) {
        // Save menu is programmed to open on Start press and something maps Select to Start.
        // So in order to ignore Select you must ignore Start too.
        buttonsToIgnore.strt = 1;
    }
}

void Commands_OverrideGameButtonInputs(btn_t* inputs) { // inputs[0]: held, inputs[1]: pressed
    if (buttonsToIgnore.val) {
        if (inputs[0].val & buttonsToIgnore.val) {
            inputs[0].val &= ~buttonsToIgnore.val;
            inputs[1].val &= ~buttonsToIgnore.val;
        } else {
            buttonsToIgnore.val = 0;
        }
    }
}
