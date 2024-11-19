#include "z3D/z3D.h"
#include "actors/player.h"
#include "menus/scene.h"
#include "camera.h"

ActorInit vanillaActorInit_Player = {0};
SamPlusUnk* Player_SamPlusUnk = NULL;

void PlayerActor_rInit(Actor* thisx, GlobalContext* globalCtx) {
    vanillaActorInit_Player.init(thisx, globalCtx);
}

void PlayerActor_rUpdate(Actor* thisx, GlobalContext* globalCtx) {
    if (noClip || (freeCam.enabled && !freeCam.locked))
        return;

    // Restore custom draw function if it's overwritten by FW spawn animation.
    if (thisx->draw == vanillaActorInit_Player.draw) {
        vanillaActorInit_Player.draw = PlayerActor_rDraw;
    }

    vanillaActorInit_Player.update(thisx, globalCtx);
}

void PlayerActor_rDestroy(Actor* thisx, GlobalContext* globalCtx) {
    vanillaActorInit_Player.destroy(thisx, globalCtx);
}

void PlayerActor_rDraw(Actor* thisx, GlobalContext* globalCtx) {
    vanillaActorInit_Player.draw(thisx, globalCtx);
    Player_SamPlusUnk = &gMainClass->sub180.saModels3DList[gMainClass->sub180.saModels3DCount - 1];
}
