#include "actor_id.h"
#include "z3D/z3D.h"
#include "common.h"

// #define NULL ((void*)0)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

#define BGCHECK_Y_MIN -32000.0f
typedef s32 (*WaterBox_GetSurfaceImpl_proc)(struct GlobalContext* globalCtx, CollisionContext* colCtx, f32 x, f32 z,
    f32* ySurface, void** outWaterBox) __attribute__((pcs("aapcs-vfp")));
// Returns true if point is within the xz boundaries of an active water box, else false
#define WaterBox_GetSurfaceImpl ((WaterBox_GetSurfaceImpl_proc)(0x35E8A0))
typedef f32 (*BgCheck_RaycastDown1_proc)(CollisionContext* colCtx, CollisionPoly* outGroundPoly, Vec3f* pos)
__attribute__((pcs("aapcs-vfp")));
// Returns the yIntersect of the nearest poly found directly below `pos`, or BGCHECK_Y_MIN if no floor detected
#define BgCheck_RaycastDown1 ((BgCheck_RaycastDown1_proc)(0x257054))

#define SurfaceType_IsLoadingZoneOrVoidPlane(surfaceType)                                                    \
    (SurfaceType_GetFloorProperty(surfaceType) == 0x5 || SurfaceType_GetFloorProperty(surfaceType) == 0xC || \
     SurfaceType_GetExitIndex(surfaceType) != 0x0 || SurfaceType_GetFloorType(surfaceType) == 0x9)

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

enum LocType {
    ABOVE_GROUND, // Location is on or above walkable ground.
    ABOVE_VOID,   // Location is over a void plane. Enemy must be able to fly.
    UNDERWATER,   // Location is underwater. Enemy must be defeatable with hookshot and iron boots.
    ABOVE_WATER,  // Location is in the air above a water surface. Enemy must be able to float or fly.
    SHALLOW_WATER,// Location is above shallow water.
    SPAWNER,      // Location is a grounded enemy spawner (stalchildren, leevers).
};

// typedef struct EnemyLocation {
//     union {
//         u16 arr[2];
//         struct {
//             u16 locType;
//             u16 actorId;
//         };
//     };
// } EnemyLocation;

typedef struct EnemyLocation {
    u16 locType;
    u16 actorId;
} EnemyLocation;

extern EnemyLocation enemyLocations[100][4][30][100];
extern EnemyType enemyTypes[47];
extern s32 rSceneLayer;

void EnemyLocations_Init(void);
void Enemizer_ChangeRoom(void);
void Enemizer_OnPlayDestroy(void);
