#include "z3D/z3D.h"
#include "actors/player.h"
#include "menus/scene.h"
#include "camera.h"
#include "input.h"
#include "draw.h"

ActorInit vanillaActorInit_Player = {0};

void PlayerActor_rInit(Actor* thisx, GlobalContext* globalCtx) {
    vanillaActorInit_Player.init(thisx, globalCtx);
}

void PlayerActor_rUpdate(Actor* thisx, GlobalContext* globalCtx) {
    if (noClip || (freeCam.enabled && !freeCam.locked))
        return;

    // if (rInputCtx.cur.zr) {
    //     thisx->world.pos.z = 297.99;
    // }

    vanillaActorInit_Player.update(thisx, globalCtx);
}

void PlayerActor_rDestroy(Actor* thisx, GlobalContext* globalCtx) {
    vanillaActorInit_Player.destroy(thisx, globalCtx);
}

void PlayerActor_rDraw(Actor* thisx, GlobalContext* globalCtx) {
    vanillaActorInit_Player.draw(thisx, globalCtx);
}
