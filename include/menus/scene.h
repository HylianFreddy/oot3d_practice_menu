#pragma once

#include "menu.h"
#include "z3D/z3Dactor.h"

enum HideEntitiesEnum {
    HIDEENTITIES_ROOMS,
    HIDEENTITIES_ACTORS,
};

enum FreeCamSetting {
    FREECAMSETTING_ENABLE,
    FREECAMSETTING_LOCK,
    FREECAMSETTING_MODE,
    FREECAMSETTING_BEHAVIOR,
    FREECAMSETTING_REMEMBER_POS,
};

enum RoomSelectorOptions {
    ROOMSELECTOR_NUMBER,
    ROOMSELECTOR_LOAD,
};

extern Menu SceneMenu;
extern ToggleMenu HideEntitiesMenu;

extern u8 noClip;
extern u8 waitingButtonRelease;
extern u8 haltActors;

void Scene_RoomSelectorMenuShow(void);
void Scene_HideEntitiesMenuShow(void);
void Scene_FreeCamSettingsMenuShow(void);
void Scene_SetEntrancePoint(void);
void Scene_SelectRoomNumber(void);
void Scene_LoadRoom(void);
void Scene_SetFlags(void);
void Scene_ClearFlags(void);
void Scene_NoClipToggle(void);
void Scene_NoClipDescription(void);
void Scene_FreeCamDescription(void);
void Scene_ToggleFreeCamSetting(s32 selected);
void Scene_HideRoomsToggle(s32 selected);
void Scene_HideActorsToggle(s32 selected);
