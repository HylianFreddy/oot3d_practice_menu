#include "colview.h"
#include "common.h"
#include "menu.h"
#include "menus.h"
#include "input.h"
#include "menus/debug.h"
#include "menus/scene.h"

#define ABS(x) ((x) >= 0 ? (x) : -(x))

static void ColView_DrawAllFromNode(u16 nodeId, SSNode* nodeTbl, SurfaceType* surfaceTypeList, s32 isDyna);
static ColViewPoly ColView_BuildColViewPoly(CollisionPoly* colPoly, SurfaceType* surfaceTypeList, s32 isDyna);
static void ColView_DrawPoly(ColViewPoly poly);
static s32 ColView_ShouldDrawPoly(ColViewPoly poly);
static Vec3f ColView_GetVtxPos(u16 vtxIdx, s32 isDyna);
static void ColView_DrawPolyForInvisibleSeam(ColViewPoly* poly);

void ColView_DrawCollision(void) {
    if (!Scene_GetCollisionOption(COLVIEW_SHOW_COLLISION) || Version_KOR || Version_TWN) {
        return;
    }

    if (Scene_GetCollisionOption(COLVIEW_DYNAMIC)) {
        DynaCollisionContext* dyna = &gGlobalContext->colCtx.dyna;
        SSNode* dynaNodeTbl = dyna->polyNodes.tbl;

        for (s32 i = 0; i < BG_ACTOR_MAX; i++) {
            if (!(dyna->bgActorFlags[i] & BGACTOR_IN_USE)) {
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
    }

    if (Scene_GetCollisionOption(COLVIEW_STATIC)) {
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
    }

    CitraPrint("%d / %d", gMainClass->sub32A0.polyCounter, gMainClass->sub32A0.polyMax);
}

static void ColView_DrawAllFromNode(u16 nodeId, SSNode* nodeTbl, SurfaceType* surfaceTypeList, s32 isDyna) {
    while (nodeId != 0xFFFF) {
        SSNode node = nodeTbl[nodeId];
        CollisionPoly* colPoly = isDyna ? &gGlobalContext->colCtx.dyna.polyList[node.polyId].colPoly
                                        : &gGlobalContext->colCtx.stat.colHeader->polyList[node.polyId];
        ColViewPoly viewPoly = ColView_BuildColViewPoly(colPoly, surfaceTypeList, isDyna);
        if (ColView_ShouldDrawPoly(viewPoly)) {
            ColView_DrawPoly(viewPoly);
        }
        if (Scene_GetCollisionOption(COLVIEW_INVISIBLE_SEAMS)) {
            ColView_DrawPolyForInvisibleSeam(&viewPoly);
        }
        nodeId = node.next;
    }
}

static ColViewPoly ColView_BuildColViewPoly(CollisionPoly* colPoly, SurfaceType* surfaceTypeList, s32 isDyna) {
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
        .verts = {
            ColView_GetVtxPos(colPoly->vtxData[0], isDyna),
            ColView_GetVtxPos(colPoly->vtxData[1], isDyna),
            ColView_GetVtxPos(colPoly->vtxData[2], isDyna),
        },
        .norm = normal,
        .dist = colPoly->dist,
        .color = color,
    };
}

static void ColView_DrawPoly(ColViewPoly poly) {
    // max count is 64 polys
    Collider_DrawPolyImpl(&gMainClass->sub32A0, &poly.verts[0], &poly.verts[1], &poly.verts[2], &poly.color);
}

#define MAX_PLANE_DIST 150
#define MAX_VERT_DIST 150
static s32 ColView_ShouldDrawPoly(ColViewPoly poly) {
    // Check if poly is invisible
    if (poly.color.a == 0.0) {
        return FALSE;
    }

    // Check if view point is behind the poly (back-face culling)
    Vec3f eye = gGlobalContext->view.eye;
    if (poly.norm.x * eye.x + poly.norm.y * eye.y + poly.norm.z * eye.z + poly.dist <= 0) {
        return FALSE;
    }

    // TODO: check if camera is looking at poly

    // Check if player is far from the poly's plane
    Vec3f pos = PLAYER->actor.world.pos;
    if (ABS(poly.norm.x * pos.x + poly.norm.y * pos.y + poly.norm.z * pos.z + poly.dist) > MAX_PLANE_DIST) {
        return FALSE;
    }

    // Check if player is far from the closest point of the poly, for each axis
    // TODO: improve this
    f32(*playerCoords)[3] = (f32(*)[3])&PLAYER->actor.world.pos;
    f32(*polyVertsCoords)[3][3] = (f32(*)[3][3])&poly.verts;
    for (s32 i = 0; i < 3; i++) {
        f32 vADist = (*polyVertsCoords)[0][i] - (*playerCoords)[i];
        f32 vBDist = (*polyVertsCoords)[1][i] - (*playerCoords)[i];
        f32 vCDist = (*polyVertsCoords)[2][i] - (*playerCoords)[i];

        if (((vADist < 0 && vBDist < 0 && vCDist < 0) ||
            (vADist > 0 && vBDist > 0 && vCDist > 0)) &&
            (MIN(MIN(ABS(vADist), ABS(vBDist)), ABS(vCDist)) > MAX_VERT_DIST)) {
            return FALSE;
        }
    }

    return TRUE;
}

static Vec3f ColView_GetVtxPos(u16 vtxIdx, s32 isDyna) {
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
    if ((!isDyna && !gStaticContext.renderGeometryDisable) ||
        (isDyna && !HideEntitiesMenu.items[HIDEENTITIES_ACTORS].on)) {
        // Try to avoid z-fighting issues by considering each
        // vertex closer to the view point than it really is.
        Vec3f eye = gGlobalContext->view.eye;
        pos.x = (pos.x * 49 + eye.x) / 50;
        pos.y = (pos.y * 49 + eye.y) / 50;
        pos.z = (pos.z * 49 + eye.z) / 50;
    }
    return pos;
}

static void ColView_DrawPolyForInvisibleSeam(ColViewPoly* poly) {
    const f32 EPSILON_OOT3D = 0.00008;
    const f32 EPSILON_OOT = 0.008;

    Vec3f norm = poly->norm;

    if (norm.y > EPSILON_OOT3D && norm.y < EPSILON_OOT) { // change check
        u8 pairs[3][2] = {{0,1},{1,2},{2,0}};
        for (s32 p = 0; p < 3; p++) {
            u8* pair = pairs[p];
            Vec3f v1 = poly->verts[pair[0]];
            Vec3f v2 = poly->verts[pair[1]];

            s32 edge_d_z = v2.z - v1.z;
            s32 edge_d_x = v2.x - v1.x;

            if (edge_d_z != 0 && edge_d_x != 0) {
                f32 extend_1_y = -((norm.x * v1.x) + (norm.z * v1.z) + poly->dist) / norm.y;
                f32 extend_2_y = -((norm.x * v2.x) + (norm.z * v2.z) + poly->dist) / norm.y;

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
                    .verts = {
                        v1,
                        v2,
                        v1ext,
                    },
                    .norm = norm,
                    .color = {
                        .r = 1.0f,
                        .g = 0.0f,
                        .b = 1.0f,
                        .a = poly->color.a,
                    },
                });

                ColView_DrawPoly((ColViewPoly){
                    .verts = {
                        v2,
                        v1ext,
                        v2ext,
                    },
                    .norm = norm,
                    .color = {
                        .r = 1.0f,
                        .g = 0.0f,
                        .b = 1.0f,
                        .a = poly->color.a,
                    },
                });
            }
        }
    }
}
