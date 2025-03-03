#include "actor_id.h"
#include "z3D/z3D.h"
#include "common.h"

// #define NULL ((void*)0)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

typedef struct EnemyType {
    char* name;
    u16 actorId;
} EnemyType;

typedef struct {
    /* 0x00 */ u16 id;
    /* 0x02 */ Vec3s pos;
    /* 0x08 */ Vec3s rot;
    /* 0x0E */ u16 params;
} ActorEntry; // size = 0x10

extern u16 enemyLocations[100][4][30][100];
extern EnemyType enemyTypes[58];
extern s32 rSceneLayer;

void EnemyLocations_Init(void);
void Enemizer_ChangeRoom(void);
void Enemizer_OnPlayDestroy(void);
