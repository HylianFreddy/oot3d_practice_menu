#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "z3D/z3D.h"

extern ActorInit vanillaActorInit_Player;
extern SamPlusUnk* Player_SamPlusUnk;

void PlayerActor_rInit(Actor* thisx, GlobalContext* globalCtx);
void PlayerActor_rUpdate(Actor* thisx, GlobalContext* globalCtx);
void PlayerActor_rDestroy(Actor* thisx, GlobalContext* globalCtx);
void PlayerActor_rDraw(Actor* thisx, GlobalContext* globalCtx);

#endif //_PLAYER_H_
