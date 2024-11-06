#include "colview.h"
#include "common.h"
#include "menu.h"
#include "menus.h"
#include "input.h"
#include "menus/debug.h"
#include "menus/scene.h"

#define ABS(x) ((x) >= 0 ? (x) : -(x))

Vec3f ColView_GetVtxPos(u16 vtxIdx, s32 isDyna) {
    vtxIdx &= 0x1FFF; // First 3 bits are flags
    Vec3f pos;
    if (isDyna) {
        Vec3f* vtxListF32 = gGlobalContext->colCtx.dyna.vtxList;
        pos.x = vtxListF32[vtxIdx].x;
        pos.y = vtxListF32[vtxIdx].y;
        pos.z = vtxListF32[vtxIdx].z;
    } else {
        Vec3s* vtxListS16 = gGlobalContext->colCtx.stat.colHeader->vtxList;
        pos.x = (f32)(vtxListS16[vtxIdx].x);
        pos.y = (f32)(vtxListS16[vtxIdx].y);
        pos.z = (f32)(vtxListS16[vtxIdx].z);
    }
    if (!gStaticContext.renderGeometryDisable) {
        // Try to avoid z-fighting issues by considering each
        // vertex closer to the view point than it really is.
        Vec3f eye = gGlobalContext->view.eye;
        pos.x = (pos.x * 49 + eye.x) / 50;
        pos.y = (pos.y * 49 + eye.y) / 50;
        pos.z = (pos.z * 49 + eye.z) / 50;
    }
    return pos;
}

ColViewPoly ColView_BuildColViewPoly(CollisionPoly* colPoly, SurfaceType* surfaceTypeList, s32 isDyna) {
    SurfaceType surfaceType = surfaceTypeList[colPoly->type];
    Vec3f normal = isDyna ? ((DynaCollisionPoly*)colPoly)->normF32
                          : (Vec3f){
                                .x = colPoly->norm.x / 32767.0,
                                .y = colPoly->norm.y / 32767.0,
                                .z = colPoly->norm.z / 32767.0,
                            };
    Color_RGBAf color;

    color.a = Scene_GetCollisionOption(COLVIEW_TRANSLUCENT) ? 0.5 : 1.0;

    if (Scene_GetCollisionOption(COLVIEW_POLYGON_CLASS)) {
        color.r = 1.0 * (normal.y < -0.8);
        color.g = 1.0 * (normal.y >= -0.8 && normal.y <= 0.5);
        color.b = 1.0 * (normal.y > 0.5);
    } else {
        if (SurfaceType_CanHookshot(surfaceType)) {
            color.r = 0.5;
            color.g = 0.5;
            color.b = 1.0;
        } else if (SurfaceType_GetWallType(surfaceType) > 0x1) {
            color.r = 0.75;
            color.g = 0.0;
            color.b = 0.75;
        } else if (SurfaceType_GetFloorProperty(surfaceType) == 0xC) {
            color.r = 1.0;
            color.g = 0.0;
            color.b = 0.0;
        } else if (SurfaceType_GetExitIndex(surfaceType) != 0x0 ||
                    SurfaceType_GetFloorProperty(surfaceType) == 0x5) {
            color.r = 0.0;
            color.g = 1.0;
            color.b = 0.0;
        } else if (SurfaceType_GetFloorType(surfaceType) != 0x0 ||
                    SurfaceType_GetWallDamage(surfaceType) != 0x0) {
            color.r = 0.75;
            color.g = 1.0;
            color.b = 0.75;
        } else if (SurfaceType_GetFloorEffect(surfaceType) == 0x1) {
            color.r = 1.0;
            color.g = 1.0;
            color.b = 0.5;
        } else {
            color.r = 1.0;
            color.g = 1.0;
            color.b = 1.0;
            if (Scene_GetCollisionOption(COLVIEW_REDUCED)) {
                color.a = 0.0;
            }
        }
    }
    if (Scene_GetCollisionOption(COLVIEW_SHADED)) {
        color.r -= color.r * ABS(0.30 * normal.y + 0.25 * normal.z);
        color.g -= color.g * ABS(0.30 * normal.y + 0.25 * normal.z);
        color.b -= color.b * ABS(0.30 * normal.y + 0.25 * normal.z);
    }

    return (ColViewPoly){
        .vA = ColView_GetVtxPos(colPoly->vtxData[0], isDyna),
        .vB = ColView_GetVtxPos(colPoly->vtxData[1], isDyna),
        .vC = ColView_GetVtxPos(colPoly->vtxData[2], isDyna),
        .norm = normal,
        .dist = colPoly->dist,
        .color = color,
    };
}

#define MAX_PLANE_DIST 150
#define MAX_VERT_DIST 150
s32 ColView_ShouldDrawPoly(ColViewPoly poly) {
    if (poly.color.a == 0.0) {
        // Poly is invisible
        return FALSE;
    }

    Vec3f eye = gGlobalContext->view.eye;
    if (poly.norm.x * eye.x + poly.norm.y * eye.y + poly.norm.z * eye.z + poly.dist <= 0) {
        // View point is behind the poly (back-face culling)
        return FALSE;
    }

    // TODO: check if camera is looking at poly

    Vec3f pos = PLAYER->actor.world.pos;
    if (ABS(poly.norm.x * pos.x + poly.norm.y * pos.y + poly.norm.z * pos.z + poly.dist) > MAX_PLANE_DIST) {
        // Player is far from the poly's plane
        return FALSE;
    }

    f32 vAxDiff = poly.vA.x - PLAYER->actor.world.pos.x;
    f32 vBxDiff = poly.vB.x - PLAYER->actor.world.pos.x;
    f32 vCxDiff = poly.vC.x - PLAYER->actor.world.pos.x;

    if (((vAxDiff < 0 && vBxDiff < 0 && vCxDiff < 0) ||
        (vAxDiff > 0 && vBxDiff > 0 && vCxDiff > 0)) &&
        (MIN(MIN(ABS(vAxDiff), ABS(vBxDiff)), ABS(vCxDiff)) > MAX_VERT_DIST)) {
        // Player is far from the closest point of the poly, on the X axis
        return FALSE;
    }

    f32 vAyDiff = poly.vA.y - PLAYER->actor.world.pos.y;
    f32 vByDiff = poly.vB.y - PLAYER->actor.world.pos.y;
    f32 vCyDiff = poly.vC.y - PLAYER->actor.world.pos.y;

    if (((vAyDiff < 0 && vByDiff < 0 && vCyDiff < 0) ||
        (vAyDiff > 0 && vByDiff > 0 && vCyDiff > 0)) &&
        (MIN(MIN(ABS(vAyDiff), ABS(vByDiff)), ABS(vCyDiff)) > MAX_VERT_DIST)) {
        // Player is far from the closest point of the poly, on the Y axis
        return FALSE;
    }

    f32 vAzDiff = poly.vA.z - PLAYER->actor.world.pos.z;
    f32 vBzDiff = poly.vB.z - PLAYER->actor.world.pos.z;
    f32 vCzDiff = poly.vC.z - PLAYER->actor.world.pos.z;

    if (((vAzDiff < 0 && vBzDiff < 0 && vCzDiff < 0) ||
        (vAzDiff > 0 && vBzDiff > 0 && vCzDiff > 0)) &&
        (MIN(MIN(ABS(vAzDiff), ABS(vBzDiff)), ABS(vCzDiff)) > MAX_VERT_DIST)) {
        // Player is far from the closest point of the poly, on the Z axis
        return FALSE;
    }

    return TRUE;
}

static void ColView_DrawPoly(ColViewPoly poly) {
    // max count is 64 polys
    Collider_DrawPolyImpl(&gMainClass->sub32A0, &poly.vA, &poly.vB, &poly.vC, &poly.color);
}

void ColView_DrawPolyForInvisibleSeam(CollisionPoly* colPoly) {
    // const f32 EPSILON_OOT3D = 0.00008;
    // const f32 EPSILON_OOT = 0.008;

    // Vec3f normal = ColView_GetNormal(colPoly);

    // if (normal.y > EPSILON_OOT3D && normal.y < EPSILON_OOT) {
    //     u8 pairs[3][2] = {{0,1},{1,2},{2,0}};
    //     for (s32 p = 0; p < 3; p++) {
    //         u8* pair = pairs[p];
    //         Vec3s vtx1 = sVtxList[colPoly->vtxData[pair[0] & 0x1FFF]];
    //         Vec3s vtx2 = sVtxList[colPoly->vtxData[pair[1] & 0x1FFF]];

    //         s32 edge_d_z = vtx2.z - vtx1.z;
    //         s32 edge_d_x = vtx2.x - vtx1.x;

    //         if (edge_d_z != 0 && edge_d_x != 0) {
    //             f32 extend_1_y = -((normal.x * vtx1.x) + (normal.z * vtx1.z) + colPoly->dist) / normal.y;
    //             f32 extend_2_y = -((normal.x * vtx2.x) + (normal.z * vtx2.z) + colPoly->dist) / normal.y;

    //             Vec3f v1 = ColView_GetVtxPos(colPoly, pair[0]);
    //             Vec3f v2 = ColView_GetVtxPos(colPoly, pair[1]);
    //             Vec3f v1ext = (Vec3f){
    //                 .x = v1.x,
    //                 .y = extend_1_y,
    //                 .z = v1.z,
    //             };
    //             Vec3f v2ext = (Vec3f){
    //                 .x = v2.x,
    //                 .y = extend_2_y,
    //                 .z = v2.z,
    //             };

    //             // CitraPrint("%f %f, %f %f", v1.y, v2.y, extend_1_y, extend_2_y);

    //             // Don't draw seams that extend downwards or only a few units above the poly
    //             if (extend_1_y < v1.y + 50.0 && extend_2_y < v2.y + 50.0) {
    //                 continue;
    //             }

    //             ColView_DrawPoly((ColViewPoly){
    //                 .vA = v1,
    //                 .vB = v2,
    //                 .vC = v1ext,
    //                 .norm = ColView_GetNormal(colPoly),
    //                 .color = {
    //                     .r = 1.0f,
    //                     .g = 0.0f,
    //                     .b = 1.0f,
    //                     .a = 0.50f,
    //                 },
    //             });

    //             ColView_DrawPoly((ColViewPoly){
    //                 .vA = v2,
    //                 .vB = v1ext,
    //                 .vC = v2ext,
    //                 .norm = ColView_GetNormal(colPoly),
    //                 .color = {
    //                     .r = 1.0f,
    //                     .g = 0.0f,
    //                     .b = 1.0f,
    //                     .a = 0.50f,
    //                 },
    //             });
    //         }
    //     }
    // }
}


void ColView_DrawAllFromNode(u16 nodeId, SSNode* nodeTbl, SurfaceType* surfaceTypeList, s32 isDyna) {
    while (nodeId != 0xFFFF) {
        SSNode node = nodeTbl[nodeId];
        CollisionPoly* colPoly = isDyna ? &gGlobalContext->colCtx.dyna.polyList[node.polyId].colPoly
                                        : &gGlobalContext->colCtx.stat.colHeader->polyList[node.polyId];
        ColViewPoly viewPoly = ColView_BuildColViewPoly(colPoly, surfaceTypeList, isDyna);
        if (ColView_ShouldDrawPoly(viewPoly)) {
            ColView_DrawPoly(viewPoly);
        }
        nodeId = node.next;
    }
}

#define MAX_BGACTOR_DIST 1000
void ColView_DrawCollision(void) {
    if (!Scene_GetCollisionOption(COLVIEW_STATIC_COLLISION) || Version_KOR || Version_TWN) {
        return;
    }

    DynaCollisionContext* dyna = &gGlobalContext->colCtx.dyna;
    SSNode* dynaNodeTbl = dyna->polyNodes.tbl;

    for (s32 i = 0; i < BG_ACTOR_MAX; i++) {
        if (!(dyna->bgActorFlags[i] & BGACTOR_IN_USE)
            // || dyna->bgActors[i].actor->xzDistToPlayer > MAX_BGACTOR_DIST
            // || dyna->bgActors[i].actor->yDistToPlayer > MAX_BGACTOR_DIST
            ) {
            continue;
        }

        DynaLookup* dynaLookup = &dyna->bgActors[i].dynaLookup;
        SurfaceType* dynaSurfaceTypeList = dyna->bgActors[i].colHeader->surfaceTypeList;
        if (dynaLookup != NULL) {
            ColView_DrawAllFromNode(dynaLookup->floor.head, dynaNodeTbl, dynaSurfaceTypeList, TRUE);
            ColView_DrawAllFromNode(dynaLookup->wall.head, dynaNodeTbl, dynaSurfaceTypeList, TRUE);
            ColView_DrawAllFromNode(dynaLookup->ceiling.head, dynaNodeTbl, dynaSurfaceTypeList, TRUE);
        }
    }

    StaticCollisionContext* stat = &gGlobalContext->colCtx.stat;
    SSNode* statNodeTbl = stat->polyNodes.tbl;
    SurfaceType* statSurfaceTypeList = stat->colHeader->surfaceTypeList;
    Vec3i sector = { 0 };
    BgCheck_GetStaticLookupIndicesFromPos(&gGlobalContext->colCtx, &PLAYER->actor.world.pos, &sector);
    s32 lookupId =
        sector.x +
        (sector.y * stat->subdivAmount.x) +
        (sector.z * stat->subdivAmount.x * stat->subdivAmount.y);
    StaticLookup* staticLookup = &stat->lookupTbl[lookupId];
    if (staticLookup != NULL) {
        ColView_DrawAllFromNode(staticLookup->floor.head, statNodeTbl, statSurfaceTypeList, FALSE);
        ColView_DrawAllFromNode(staticLookup->wall.head, statNodeTbl, statSurfaceTypeList, FALSE);
        ColView_DrawAllFromNode(staticLookup->ceiling.head, statNodeTbl, statSurfaceTypeList, FALSE);
    }

    // CitraPrint("%X: %d / %d", lookup, gMainClass->sub32A0.polyCounter, gMainClass->sub32A0.polyMax);
}
