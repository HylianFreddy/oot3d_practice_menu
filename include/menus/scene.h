#pragma once

#include "menu.h"
#include "z3D/z3Dactor.h"

enum HideEntitiesEnum {
    HIDEENTITIES_ROOMS,
    HIDEENTITIES_ACTORS,
    HIDEENTITIES_EXCEPT_LINK,
    HIDEENTITIES_MAX,
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

void Scene_NoClipToggle(void);
