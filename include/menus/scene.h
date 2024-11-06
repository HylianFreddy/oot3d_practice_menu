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
    COLVIEW_TRANSLUCENT,
    COLVIEW_POLYGON_CLASS,
    COLVIEW_SHADED,
    COLVIEW_REDUCED,
    COLVIEW_SHOW_COLLIDERS,
    COLVIEW_MAX,
};

extern Menu SceneMenu;
extern ToggleMenu HideEntitiesMenu;
extern ToggleMenu CollisionMenu;
#define Scene_GetCollisionOption(option) (CollisionMenu.items[option].on)

extern u8 noClip;
extern u8 waitingButtonRelease;
extern u8 haltActors;

extern void Scene_RoomSelectorMenuShow(void);
extern void Scene_HideEntitiesMenuShow(void);
extern void Scene_FreeCamSettingsMenuShow(void);
extern void Scene_CollisionMenuShow(void);

extern void Scene_SetEntrancePoint(void);
extern void Scene_SelectRoomNumber(void);
extern void Scene_LoadRoom(void);
extern void Scene_SetFlags(void);
extern void Scene_ClearFlags(void);
extern void Scene_NoClipToggle(void);
extern void Scene_NoClipDescription(void);
extern void Scene_FreeCamDescription(void);
extern void Scene_ToggleFreeCamSetting(s32 selected);
extern void Scene_HideRoomsToggle(s32 selected);
extern void Scene_HideActorsToggle(s32 selected);
extern void Scene_ToggleCollisionOption(s32 selected);
