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

enum ColViewOptions {
    COLVIEW_SHOW_COLLISION,
    COLVIEW_STATIC,
    COLVIEW_DYNAMIC,
    COLVIEW_INVISIBLE_SEAMS,
    COLVIEW_TRANSLUCENT,
    COLVIEW_POLYGON_CLASS,
    COLVIEW_SHADED,
    COLVIEW_REDUCED,
    COLVIEW_POLY_COUNT_MENU,
    COLVIEW_SHOW_COLLIDERS,
    COLVIEW_AT,
    COLVIEW_AC,
    COLVIEW_OC,
    COLVIEW_MAX,
};

extern Menu SceneMenu;
extern ToggleMenu HideEntitiesMenu;
extern ToggleMenu CollisionMenu;
#define Scene_GetCollisionOption(option) (CollisionMenu.items[option].on)

extern u8 noClip;
extern u8 waitingButtonRelease;
extern u8 haltActors;

void Scene_RoomSelectorMenuShow(void);
void Scene_HideEntitiesMenuShow(void);
void Scene_FreeCamSettingsMenuShow(void);
void Scene_CollisionMenuShow(void);
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
void Scene_ToggleCollisionOption(s32 selected);
