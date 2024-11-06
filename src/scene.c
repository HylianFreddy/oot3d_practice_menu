#include "menu.h"
#include "menus/scene.h"
#include "draw.h"
#include "z3D/z3D.h"
#include "common.h"
#include "input.h"
#include "camera.h"
#include "advance.h"
#include "menus/commands.h"
#include "colview.h"

u8 noClip = 0;
u8 waitingButtonRelease = 0;
u8 haltActors = 0;

static s32 selectedRoomNumber = -1;

ToggleMenu CollisionMenu = {
    "Collision",
    .nbItems = COLVIEW_MAX,
    .initialCursorPos = 0,
    {
        {1, "Show Collision (WIP)", .method = Scene_ToggleCollisionOption},
        {1, " Static Collision", .method = Scene_ToggleCollisionOption},
        {1, " Dynamic Collision", .method = Scene_ToggleCollisionOption},
        {1, "  Translucent", .method = Scene_ToggleCollisionOption},
        {0, "  Polygon Class", .method = Scene_ToggleCollisionOption},
        {1, "  Shaded", .method = Scene_ToggleCollisionOption},
        {0, "  Reduced", .method = Scene_ToggleCollisionOption},
        {0, "Show Colliders", .method = Scene_ToggleCollisionOption},
    }
};

static Menu RoomSelectorMenu = {
    "Room Selector",
    .nbItems = 2,
    .initialCursorPos = 0,
    {
        {"Room Number: --", METHOD, .method = Scene_SelectRoomNumber},
        {"Load", METHOD, .method = Scene_LoadRoom},
    }
};

static Menu FreeCamMenu = {
    "Free Camera",
    .nbItems = 2,
    .initialCursorPos = 0,
    {
        {"Settings", METHOD, .method = Scene_FreeCamSettingsMenuShow},
        {"Info", METHOD, .method = Scene_FreeCamDescription},
    }
};

ToggleMenu FreeCamSettingsMenu = {
    "Free Camera Settings",
    .nbItems = 5,
    .initialCursorPos = 0,
    {
        {0, "Enable", .method = Scene_ToggleFreeCamSetting},
        {0, "Lock", .method = Scene_ToggleFreeCamSetting},
        {0, "Mode: OFF=Camera / ON=View", .method = Scene_ToggleFreeCamSetting},
        {0, "Behavior: OFF=Manual / ON=Radial", .method = Scene_ToggleFreeCamSetting},
        {0, "Remember Position", .method = Scene_ToggleFreeCamSetting},
    }
};

ToggleMenu HideEntitiesMenu = {
    "Hide Game Entities",
    .nbItems = 2,
    .initialCursorPos = 0,
    {
        {0, "Hide Rooms", .method = Scene_HideRoomsToggle},
        {0, "Hide Actors", .method = Scene_HideActorsToggle},
    }
};

Menu SceneMenu = {
    "Scene",
    .nbItems = 8,
    .initialCursorPos = 0,
    {
        {"NoClip / Move Link", METHOD, .method = Scene_NoClipDescription},
        {"Set Entrance Point", METHOD, .method = Scene_SetEntrancePoint},
        {"Set Flags", METHOD, .method = Scene_SetFlags},
        {"Clear Flags", METHOD, .method = Scene_ClearFlags},
        {"Room Selector", METHOD, .method = Scene_RoomSelectorMenuShow},
        {"Collision", METHOD, .method = Scene_CollisionMenuShow},
        {"Free Camera", MENU, .menu = &FreeCamMenu},
        {"Hide Game Entities", METHOD, .method = Scene_HideEntitiesMenuShow},
    }
};

void Scene_CollisionMenuShow(void) {
    CollisionMenu.items[COLVIEW_SHOW_COLLIDERS].on = gStaticContext.collisionDisplay;
    ToggleMenuShow(&CollisionMenu);
}

void Scene_ToggleCollisionOption(s32 selected) {
    CollisionMenu.items[selected].on ^= 1;
    switch (selected) {
        case COLVIEW_SHOW_COLLIDERS:
            gStaticContext.collisionDisplay ^= 1;
            break;
    }
}

void Scene_SetEntrancePoint(void) {
    gSaveContext.respawn[0] = (RespawnData){
        PLAYER->actor.world.pos,
        PLAYER->actor.shape.rot.y,
        (ADDITIONAL_FLAG_BUTTON ? 0x0EFF : 0x0DFF),
        gSaveContext.entranceIndex,
        gGlobalContext->roomCtx.curRoom.num,
        0,
        gGlobalContext->actorCtx.flags.tempSwch,
        gGlobalContext->actorCtx.flags.tempCollect,
    };
    setAlert(ADDITIONAL_FLAG_BUTTON ? "EP set as from EPG" : "EP set!", 90);
}

static void Scene_RoomSelectorUpdateNumber(void) {
    // Write current room number to menu item title
    snprintf(RoomSelectorMenu.items[ROOMSELECTOR_NUMBER].title + 13, 3, "%2.2d", selectedRoomNumber);
}

void Scene_RoomSelectorMenuShow(void) {
    if (!isInGame()) {
        setAlert("Not in game", 90);
        return;
    }
    selectedRoomNumber = gGlobalContext->roomCtx.curRoom.num;
    RoomSelectorMenu.initialCursorPos = 0; // reset the cursor because it's useless to keep it on "Load"
    Scene_RoomSelectorUpdateNumber();
    menuShow(&RoomSelectorMenu);
}

void Scene_SelectRoomNumber(void) {
    if (gGlobalContext->numRooms > 1) {
        Menu_EditAmount(30 + 12 * SPACING_X, 30, &selectedRoomNumber, VARTYPE_U16, 0, gGlobalContext->numRooms - 1, 2,
                        false, NULL, 0);
    }
    Scene_RoomSelectorUpdateNumber();
    if (ADDITIONAL_FLAG_BUTTON) { // preserve old behavior of setting the room number in the Entrance Point
        gSaveContext.respawn[0].roomIndex = selectedRoomNumber;
        setAlert("Set in EP", 90);
    }
}

void Scene_LoadRoom(void) {
#if Version_KOR || Version_TWN
    setAlert(UNSUPPORTED_WARNING, 90);
    return;
#endif
    if (selectedRoomNumber == gGlobalContext->roomCtx.curRoom.num) {
        setAlert("Already loaded", 90);
        return;
    }
    if (gGlobalContext->roomCtx.prevRoom.num != -1) {
        // For now, let's just refuse loading rooms while the game is already in a room transition state.
        setAlert("roomCtx busy!", 90);
        return;
    }
    Room_StartTransition(gGlobalContext, &gGlobalContext->roomCtx, selectedRoomNumber);
    Room_ClearPrevRoom(gGlobalContext, &gGlobalContext->roomCtx);
    menuOpen = false;
}

void Scene_SetFlags(void) {
    gGlobalContext->actorCtx.flags.swch = 0xFFFFFFFF;
    gGlobalContext->actorCtx.flags.tempSwch = 0xFFFFFFFF;
    gGlobalContext->actorCtx.flags.chest = 0xFFFFFFFF;
    gGlobalContext->actorCtx.flags.clear = 0xFFFFFFFF;
    gGlobalContext->actorCtx.flags.tempClear = 0xFFFFFFFF;
    gGlobalContext->actorCtx.flags.collect = 0xFFFFFFFF;
    gGlobalContext->actorCtx.flags.tempCollect = 0xFFFFFFFF;
}

void Scene_ClearFlags(void) {
    gGlobalContext->actorCtx.flags.swch = 0;
    gGlobalContext->actorCtx.flags.tempSwch = 0;
    gGlobalContext->actorCtx.flags.chest = 0;
    gGlobalContext->actorCtx.flags.clear = 0;
    gGlobalContext->actorCtx.flags.tempClear = 0;
    gGlobalContext->actorCtx.flags.collect = 0;
    gGlobalContext->actorCtx.flags.tempCollect = 0;
}

void Scene_NoClipToggle(void) {
    if (isInGame() && !FreeCam_Moving) {
        if (!noClip) {
            haltActors = 1;
            noClip = 1;
            if (advance_ctx.advance_state == PAUSED) {
                // Unpause automatically when entering NoClip
                pauseUnpause = 1;
            }
        }
        else {
            haltActors = 0;
            noClip = 0;
        }
        menuOpen = false;
        waitingButtonRelease = 1;
    }
}

void Scene_NoClipDescription(void) {

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_DrawString(10, 10, COLOR_TITLE, "NoClip");
    Draw_DrawString(30, 30, COLOR_WHITE, "Move Link freely in the 3D space, bypassing\n"
                                        "collision detection.\n"
                                        "Press A to start, B to cancel.\n\n"
                                        "Commands:\n"
                                        "Circle Pad - Move horizontally (camera)\n"
                                        "DPad       - Move horizontally (cardinal)\n"
                                        "L/R        - Move vertically\n"
                                        "Hold X     - Move fast\n"
                                        "Y          - Freeze/Unfreeze actors\n"
                                        "A          - Quit and confirm position\n"
                                        "B          - Quit and cancel movement");
    Draw_FlushFramebuffer();
    Draw_Unlock();

    do
    {
        u32 pressed = Input_WaitWithTimeout(1000);

        if (pressed & BUTTON_B){
            break;
        }
        if (pressed & BUTTON_A){
            Scene_NoClipToggle();
        }
    }while(onMenuLoop());
}

void Scene_FreeCamSettingsMenuShow(void) {
    FreeCamSettingsMenu.items[FREECAMSETTING_ENABLE].on = freeCam.enabled;
    FreeCamSettingsMenu.items[FREECAMSETTING_LOCK].on = freeCam.locked;
    FreeCamSettingsMenu.items[FREECAMSETTING_MODE].on = freeCam.mode;
    FreeCamSettingsMenu.items[FREECAMSETTING_BEHAVIOR].on = freeCam.behavior;
    FreeCamSettingsMenu.items[FREECAMSETTING_REMEMBER_POS].on = freeCam.rememberPos;
    ToggleMenuShow(&FreeCamSettingsMenu);
}

void Scene_FreeCamDescription(void) {

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_DrawString(10, 10, COLOR_TITLE, "Free Camera");
    Draw_DrawString(30, 30, COLOR_WHITE, "Move the camera freely in 3D space, leaving\n"
                                        "Link behind.\n"
                                        "Press A to start, B to cancel.\n\n"
                                        "Commands:\n"
                                        "Circle Pad   - Move forward/sideways\n"
                                        "L+Circle Pad - Rotate in place\n"
                                        "C Stick      - Rotate while moving\n"
                                        "DPad Up/Down - Move vertically\n"
                                        "Hold X       - Move fast\n"
                                        "Y            - Freeze/Unfreeze non-player actors\n"
                                        "A            - Quit and lock camera in place\n"
                                        "B            - Quit and disable Free Camera");
    Draw_FlushFramebuffer();
    Draw_Unlock();

    do
    {
        u32 pressed = Input_WaitWithTimeout(1000);

        if (pressed & BUTTON_B){
            break;
        }
        if (pressed & BUTTON_A){
            FreeCam_Toggle();
            menuOpen = false;
            waitingButtonRelease = 1;
        }
    }while(onMenuLoop());
}

void Scene_ToggleFreeCamSetting(s32 selected) {
    switch (selected) {
        case FREECAMSETTING_ENABLE:
            FreeCam_Toggle();
            FreeCamSettingsMenu.items[selected].on = freeCam.enabled;
            break;
        case FREECAMSETTING_LOCK:
            FreeCam_ToggleLock();
            FreeCamSettingsMenu.items[selected].on = freeCam.locked;
            break;
        case FREECAMSETTING_MODE:
            FreeCamSettingsMenu.items[selected].on = freeCam.mode ^= 1;
            break;
        case FREECAMSETTING_BEHAVIOR:
            FreeCamSettingsMenu.items[selected].on = freeCam.behavior ^= 1;
            break;
        case FREECAMSETTING_REMEMBER_POS:
            FreeCamSettingsMenu.items[selected].on = freeCam.rememberPos ^= 1;
            break;
    }
}

void Scene_HideEntitiesMenuShow() {
    HideEntitiesMenu.items[HIDEENTITIES_ROOMS].on = gStaticContext.renderGeometryDisable;
    ToggleMenuShow(&HideEntitiesMenu);
}

void Scene_HideRoomsToggle(s32 selected) {
    gStaticContext.renderGeometryDisable ^= 1;
    HideEntitiesMenu.items[HIDEENTITIES_ROOMS].on ^= 1;
}

void Scene_HideActorsToggle(s32 selected) {
    HideEntitiesMenu.items[HIDEENTITIES_ACTORS].on ^= 1;
}

s32 Scene_HaltActorsEnabled() {
    return haltActors;
}
