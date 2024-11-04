#include "colview.h"
#include "common.h"
#include "menu.h"
#include "menus.h"
#include "input.h"
#include "menus/debug.h"
#include "menus/scene.h"

#define ABS(x) ((x) >= 0 ? (x) : -(x))
#define POLY_MAX_COUNT 64

static CollisionPoly* sExtraPolys[POLY_MAX_COUNT] = { 0 };
static u8 sExtraPolysCounter = 0;

Vec3f ColView_GetVtxPos(CollisionPoly* colPoly, u16 polyVtxId) {
    Vec3s* vtxList = gGlobalContext->colCtx.stat.colHeader->vtxList;
    u16 vtxIdx = colPoly->vtxData[polyVtxId] & 0x1FFF;
    Vec3f pos = {
        .x=(f32)(vtxList[vtxIdx].x),
        .y=(f32)(vtxList[vtxIdx].y),
        .z=(f32)(vtxList[vtxIdx].z),
    };
    if (!gStaticContext.renderGeometryDisable) {
        // try to avoid z-fighting issues
        pos.x += ((colPoly->norm.x > 0) ? 0.002 : -0.002);
        pos.y += ((colPoly->norm.y > 0) ? 0.002 : -0.002);
        pos.z += ((colPoly->norm.z > 0) ? 0.002 : -0.002);
    }
    return pos;
}

Vec3f ColView_GetNormal(CollisionPoly* colPoly) {
    return (Vec3f){
        .x = colPoly->norm.x / 32767.0,
        .y = colPoly->norm.y / 32767.0,
        .z = colPoly->norm.z / 32767.0,
    };
}

ColViewPoly ColView_GetColViewPoly(CollisionPoly* colPoly) {
    SurfaceType surfaceType = gGlobalContext->colCtx.stat.colHeader->surfaceTypeList[colPoly->type];
    Vec3f normal = ColView_GetNormal(colPoly);
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
        color.r -= color.r * (0.25 * normal.y + 0.25 * normal.z);
        color.g -= color.g * (0.25 * normal.y + 0.25 * normal.z);
        color.b -= color.b * (0.25 * normal.y + 0.25 * normal.z);
    }

    return (ColViewPoly){
        .vA = ColView_GetVtxPos(colPoly, 0),
        .vB = ColView_GetVtxPos(colPoly, 1),
        .vC = ColView_GetVtxPos(colPoly, 2),
        .norm = normal,
        .dist = colPoly->dist,
        .color = color,
    };
}

s32 ColView_IsPolyVisible(ColViewPoly poly) {
    Vec3f eye = gGlobalContext->view.eye;
    return poly.norm.x * eye.x + poly.norm.y * eye.y + poly.norm.z * eye.z + poly.dist > 0;

    // add check if cam is looking at poly
}

#define MAX_PLANE_DIST 150
#define MAX_VERT_DIST 150
s32 ColView_IsPolyCloseToLink(ColViewPoly poly) {
    Vec3f pos = PLAYER->actor.world.pos;
    s32 planeCheck = ABS(poly.norm.x * pos.x + poly.norm.y * pos.y + poly.norm.z * pos.z + poly.dist) < MAX_PLANE_DIST;

    s32 distCheckX;
    s32 distCheckY;
    s32 distCheckZ;

    f32 vAxDiff = poly.vA.x - PLAYER->actor.world.pos.x;
    f32 vBxDiff = poly.vB.x - PLAYER->actor.world.pos.x;
    f32 vCxDiff = poly.vC.x - PLAYER->actor.world.pos.x;

    if ((vAxDiff < 0 && vBxDiff < 0 && vCxDiff < 0) ||
        (vAxDiff > 0 && vBxDiff > 0 && vCxDiff > 0)) {
        distCheckX = (MIN(MIN(ABS(vAxDiff), ABS(vBxDiff)), ABS(vCxDiff)) < MAX_VERT_DIST);
    } else {
        distCheckX = 1;
    }

    f32 vAyDiff = poly.vA.y - PLAYER->actor.world.pos.y;
    f32 vByDiff = poly.vB.y - PLAYER->actor.world.pos.y;
    f32 vCyDiff = poly.vC.y - PLAYER->actor.world.pos.y;

    if ((vAyDiff < 0 && vByDiff < 0 && vCyDiff < 0) ||
        (vAyDiff > 0 && vByDiff > 0 && vCyDiff > 0)) {
        distCheckY = (MIN(MIN(ABS(vAyDiff), ABS(vByDiff)), ABS(vCyDiff)) < MAX_VERT_DIST);
    } else {
        distCheckY = 1;
    }

    f32 vAzDiff = poly.vA.z - PLAYER->actor.world.pos.z;
    f32 vBzDiff = poly.vB.z - PLAYER->actor.world.pos.z;
    f32 vCzDiff = poly.vC.z - PLAYER->actor.world.pos.z;

    if ((vAzDiff < 0 && vBzDiff < 0 && vCzDiff < 0) ||
        (vAzDiff > 0 && vBzDiff > 0 && vCzDiff > 0)) {
        distCheckZ = (MIN(MIN(ABS(vAzDiff), ABS(vBzDiff)), ABS(vCzDiff)) < MAX_VERT_DIST);
    } else {
        distCheckZ = 1;
    }

    return planeCheck && distCheckX && distCheckY && distCheckZ;
}

static void ColView_DrawPoly(ColViewPoly poly) {
    // max count is 64 polys
    Collider_DrawPolyImpl(&gMainClass->sub32A0, &poly.vA, &poly.vB, &poly.vC, &poly.color);
}

static void ColView_DrawPolyForInvisibleSeam(CollisionPoly* colPoly) {
    const f32 EPSILON_OOT3D = 0.00008;
    const f32 EPSILON_OOT = 0.008;
    Vec3s* vtxList = gGlobalContext->colCtx.stat.colHeader->vtxList;

    Vec3f normal = ColView_GetNormal(colPoly);

    if (normal.y > EPSILON_OOT3D && normal.y < EPSILON_OOT) {
        u8 pairs[3][2] = {{0,1},{1,2},{2,0}};
        for (s32 p = 0; p < 3; p++) {
            u8* pair = pairs[p];
            Vec3s vtx1 = vtxList[colPoly->vtxData[pair[0] & 0x1FFF]];
            Vec3s vtx2 = vtxList[colPoly->vtxData[pair[1] & 0x1FFF]];

            s32 edge_d_z = vtx2.z - vtx1.z;
            s32 edge_d_x = vtx2.x - vtx1.x;

            if (edge_d_z != 0 && edge_d_x != 0) {
                f32 extend_1_y = -((normal.x * vtx1.x) + (normal.z * vtx1.z) + colPoly->dist) / normal.y;
                f32 extend_2_y = -((normal.x * vtx2.x) + (normal.z * vtx2.z) + colPoly->dist) / normal.y;

                Vec3f v1 = ColView_GetVtxPos(colPoly, pair[0]);
                Vec3f v2 = ColView_GetVtxPos(colPoly, pair[1]);
                Vec3f v1ext = (Vec3f){
                    .x = v1.x,
                    .y = extend_1_y,
                    .z = v1.z,
                };
                Vec3f v2ext = (Vec3f){
                    .x = v2.x,
                    .y = extend_2_y,
                    .z = v2.z,
                };

                // CitraPrint("%f %f, %f %f", v1.y, v2.y, extend_1_y, extend_2_y);

                // Don't draw seams that extend downwards or only a few units above the poly
                if (extend_1_y < v1.y + 50.0 && extend_2_y < v2.y + 50.0) {
                    continue;
                }

                ColView_DrawPoly((ColViewPoly){
                    .vA = v1,
                    .vB = v2,
                    .vC = v1ext,
                    .norm = ColView_GetNormal(colPoly),
                    .color = {
                        .r = 1.0f,
                        .g = 0.0f,
                        .b = 1.0f,
                        .a = 0.50f,
                    },
                });

                ColView_DrawPoly((ColViewPoly){
                    .vA = v2,
                    .vB = v1ext,
                    .vC = v2ext,
                    .norm = ColView_GetNormal(colPoly),
                    .color = {
                        .r = 1.0f,
                        .g = 0.0f,
                        .b = 1.0f,
                        .a = 0.50f,
                    },
                });
            }
        }
    }
}

void ColView_DrawFromCollPoly(CollisionPoly* colPoly, s32 invSeam, s32 firstPass) {
    ColViewPoly viewPoly = ColView_GetColViewPoly(colPoly);
    if (viewPoly.color.a != 0.0 && ColView_IsPolyVisible(viewPoly)) {
        if (!firstPass || ColView_IsPolyCloseToLink(viewPoly)) {
            ColView_DrawPoly(viewPoly);
        } else if (firstPass && sExtraPolysCounter < POLY_MAX_COUNT) {
            sExtraPolys[sExtraPolysCounter++] = colPoly;
        }
    }
    if (invSeam) {
        ColView_DrawPolyForInvisibleSeam(colPoly);
    }
}

void ColView_DrawAllFromNode(u16 nodeId) {
    while (nodeId != 0xFFFF) {
        SSNode node = gGlobalContext->colCtx.stat.polyNodes.tbl[nodeId];
        ColView_DrawFromCollPoly(&gGlobalContext->colCtx.stat.colHeader->polyList[node.polyId], FALSE, TRUE);
        nodeId = node.next;
    }
}

void ColView_DrawAllFromLookup(StaticLookup* lookup) {
    if (lookup == 0) {
        return;
    }
    ColView_DrawAllFromNode(lookup->floor.head);
    ColView_DrawAllFromNode(lookup->wall.head);
    ColView_DrawAllFromNode(lookup->ceiling.head);
}

void ColView_DrawCollision(void) {
    if (!Scene_GetCollisionOption(COLVIEW_STATIC_COLLISION) || Version_KOR || Version_TWN) {
        return;
    }

    Vec3i sector = { 0 };
    BgCheck_GetStaticLookupIndicesFromPos(&gGlobalContext->colCtx, &PLAYER->actor.world.pos, &sector);
    StaticCollisionContext ctx = gGlobalContext->colCtx.stat;
    s32 lookupId =
        sector.x +
        (sector.y * ctx.subdivAmount.x) +
        (sector.z * ctx.subdivAmount.x * ctx.subdivAmount.y);
    StaticLookup* lookup = &ctx.lookupTbl[lookupId];
    ColView_DrawAllFromLookup(lookup);

    while ((gMainClass->sub32A0.polyCounter < gMainClass->sub32A0.polyMax) && sExtraPolysCounter > 0) {
        ColView_DrawFromCollPoly(sExtraPolys[--sExtraPolysCounter], FALSE, FALSE);
    }

    sExtraPolysCounter = 0;

    CitraPrint("%X: %d / %d", lookup, gMainClass->sub32A0.polyCounter, gMainClass->sub32A0.polyMax);
}
