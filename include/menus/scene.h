#pragma once

#include "menu.h"
#include "z3D/z3Dactor.h"

enum HideEntitiesEnum {
    HIDEENTITIES_ROOMS,
    HIDEENTITIES_ACTORS,
};

extern Menu SceneMenu;
extern ToggleMenu HideEntitiesMenu;

extern u8 noClip;
extern u8 freeCam;
extern u8 releasedNoClipButtons;
extern PosRot freeCamView;
extern u8 haltActors;

extern void Scene_RoomNumberMenuShow(void);
extern void Scene_HideEntitiesMenuShow(void);

extern void Scene_SetEntrancePoint(void);
extern void Scene_SetRoomNumberinEP(s32 selected);
extern void Scene_SetFlags(void);
extern void Scene_ClearFlags(void);
extern void Scene_NoClipToggle(void);
extern void Scene_NoClipDescription(void);
extern void Scene_FreeCamToggle(void);
extern void Scene_FreeCamDescription(void);
extern void Scene_HideRoomsToggle(s32 selected);
extern void Scene_HideActorsToggle(s32 selected);
