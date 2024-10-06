#include "colview.h"
#include "common.h"
#include "menu.h"
#include "menus.h"
#include "input.h"
#include "menus/debug.h"

#define ABS(x) ((x) >= 0 ? (x) : -(x))

ColViewPoly createDummyPoly(void) {
    return (ColViewPoly){
        .vA = {
            .x=-25.0f,
            .y=0.0f,
            .z=0.0f,
        },
        .vB = {
            .x=25.0f,
            .y=0.0f,
            .z=0.0f,
        },
        .vC = {
            .x=0.0f,
            .y=50.0f,
            .z=0.0f,
        },
        .norm = {
            .x = 0,
            .y = 0,
            .z = 0,
        },
        .dist = 0.0,
        .color = {
            .r = 1.0f,
            .g = 0.0f,
            .b = 1.0f,
            .a = 0.75f,
        },
    };
}

Vec3f ColView_GetVtxPos(CollisionPoly* colPoly, u16 polyVtxId) {
    Vec3s* vtxList = gGlobalContext->colCtx.stat.colHeader->vtxList;
    u16 vtxIdx = colPoly->vtxData[polyVtxId];
    if (polyVtxId <= 2) {
        vtxIdx &= 0x1FFF; // See CollisionPoly_GetVertices from decomp
    }
    return (Vec3f){
        .x=(f32)(vtxList[vtxIdx].x),
        .y=(f32)(vtxList[vtxIdx].y),
        .z=(f32)(vtxList[vtxIdx].z),
    };
}

Vec3f ColView_GetNormal(CollisionPoly* colPoly) {
    return (Vec3f){
        .x = colPoly->norm.x / 32767.0,
        .y = colPoly->norm.y / 32767.0,
        .z = colPoly->norm.z / 32767.0,
    };
}

ColViewPoly ColView_GetColViewPoly(CollisionPoly* colPoly) {
    // CitraPrint("");
    // if (rInputCtx.cur.zr) {
    //     memoryEditorAddress = (u32)colPoly;
    //     menuOpen = true;
    //     menuShow(&gz3DMenu);
    // }

    // CitraPrint("poly vtx A= %X", colPoly->vtxData[0]);
    // CitraPrint("poly vtx B= %X", colPoly->vtxData[1]);
    // CitraPrint("poly vtx C= %X", colPoly->vtxData[2]);
    // CitraPrint("poly A X = %X", vtxList[colPoly->vtxData[0]].x);
    // CitraPrint("poly A Y = %X", vtxList[colPoly->vtxData[0]].y);
    // CitraPrint("poly A Z = %X", vtxList[colPoly->vtxData[0]].z);
    // CitraPrint("numVertices = %X", gGlobalContext->colCtx.stat.colHeader->numVertices);
    // memoryEditorAddress = (u32)gGlobalContext->colCtx.stat.colHeader;
    // menuOpen = 1;
    // menuShow(&gz3DMenu);

    Vec3f normal = ColView_GetNormal(colPoly);
    // CitraPrint("%f %f %f", normal.x, normal.y, normal.z);

    return (ColViewPoly){
        .vA = ColView_GetVtxPos(colPoly, 0),
        .vB = ColView_GetVtxPos(colPoly, 1),
        .vC = ColView_GetVtxPos(colPoly, 2),
        .norm = normal,
        .dist = colPoly->dist,
        .color = {
            .r = 0.9 + (0.1 * normal.x),
            .g = 0.9 + (0.1 * normal.z),
            .b = 0.9 + (0.1 * normal.y), // floors -> high y normal
            .a = 0.5,
        },
    };
}

s32 ColView_IsPolyVisible(ColViewPoly poly) {
    Vec3f eye = gGlobalContext->view.eye;
    return poly.norm.x * eye.x + poly.norm.y * eye.y + poly.norm.z * eye.z + poly.dist > 0;
}

ColViewPoly getPlayerFloorPoly(void) {
    CollisionPoly* colPoly = PLAYER->actor.floorPoly;

    return (ColViewPoly){
        .vA = ColView_GetVtxPos(colPoly, 0),
        .vB = ColView_GetVtxPos(colPoly, 1),
        .vC = ColView_GetVtxPos(colPoly, 2),
        .norm = ColView_GetNormal(colPoly),
        .dist = colPoly->dist,
        .color = {
            .r = 1.0f,
            .g = 1.0f,
            .b = 1.0f,
            .a = 0.50f,
        },
    };
}

static void ColView_DrawPoly(ColViewPoly poly) {
    Collider_DrawPolyImpl((void*)0x5c1858, &poly.vA, &poly.vB, &poly.vC, &poly.color);
}

static void ColView_DrawPolyForInvisibleSeam(CollisionPoly* colPoly) {
    const f32 EPSILON_OOT3D = 0.00008;
    const f32 EPSILON_OOT = 0.008;
    Vec3s* vtxList = gGlobalContext->colCtx.stat.colHeader->vtxList;

    Vec3f normal = ColView_GetNormal(colPoly);

    if (normal.y > EPSILON_OOT3D && normal.y < EPSILON_OOT) {
        // CitraPrint("normal.y: %f", normal.y);
        // CitraPrint("poly.norm.y: %X", poly.norm.y);
        // ColView_DrawPoly(ColView_GetColViewPoly(colPoly));
        // CitraPrint("______");

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

void ColView_DrawAllFromNode(SSNode node) {
    u16 i = 0;
    while (node.next != 0xFFFF) {
        // CitraPrint("node.polyId: %X", node.polyId);
        // CitraPrint("node.next: %X", node.next);

        CollisionPoly* colPoly = &gGlobalContext->colCtx.stat.colHeader->polyList[node.polyId];

        ColViewPoly viewPoly = ColView_GetColViewPoly(colPoly);
        if (ColView_IsPolyVisible(viewPoly)) {
            ColView_DrawPoly(viewPoly);
        }
        ColView_DrawPolyForInvisibleSeam(colPoly);

        node = gGlobalContext->colCtx.stat.polyNodes.tbl[node.next];
        i++;
    }
    // CitraPrint("i max: %X", i);
}

// static u16 lookupIndex = 0;
StaticLookup* ColView_Lookup;
// static s32 sceneId = -1;
void ColView_DrawCollision(void) {
    // if (rInputCtx.cur.zr || ColView_Lookup == 0) { // || sceneId != gGlobalContext->sceneNum
    //     return;
    // }
    // if (rInputCtx.pressed.zr) {
    //     lookupIndex = (lookupIndex + 1) % 8;
    //     CitraPrint("lookupIndex: %X", lookupIndex);
    // }
    // ColView_Lookup = &gGlobalContext->colCtx.stat.lookupTbl[lookupIndex];
    // CitraPrint("numPolygons: %X", gGlobalContext->colCtx.stat.colHeader->numPolygons);

    ColViewPoly dummyPoly = createDummyPoly();
    if (ColView_IsPolyVisible(dummyPoly)) {
        ColView_DrawPoly(dummyPoly);
    }

    // for (s32 i = 0; i < gGlobalContext->colCtx.stat.colHeader->numPolygons; i++) {
    //     ColView_DrawPolyForInvisibleSeam(&gGlobalContext->colCtx.stat.colHeader->polyList[i]);
    // }
    // return;

    if (PLAYER->actor.floorPoly != 0) {
        // ColView_DrawPoly(getPlayerFloorPoly());
    }

    // static s32 subdivCount = 0;
    // Vec3i v = gGlobalContext->colCtx.stat.subdivAmount;
    // subdivCount = v.x * v.y * v.z;

    u16 floorIndex, wallIndex, ceilingIndex;
    floorIndex = ColView_Lookup->floor.head;
    wallIndex = ColView_Lookup->wall.head;
    ceilingIndex = ColView_Lookup->ceiling.head;
    // do {
    //     floorIndex = gGlobalContext->colCtx.stat.lookupTbl[lookupIndex].floor.head;
    //     wallIndex = gGlobalContext->colCtx.stat.lookupTbl[lookupIndex].wall.head;
    //     ceilingIndex = gGlobalContext->colCtx.stat.lookupTbl[lookupIndex].ceiling.head;
    //     lookupIndex = (lookupIndex + 1) % subdivCount;
    // } while (floorIndex == 0xFFFF && wallIndex == 0xFFFF && ceilingIndex == 0xFFFF);

    // u16 floorIndex = gGlobalContext->colCtx.stat.lookupTbl[lookupIndex].floor.head;
    // CitraPrint("floorIndex: %X", floorIndex);
    if (floorIndex != 0xFFFF) ColView_DrawAllFromNode(gGlobalContext->colCtx.stat.polyNodes.tbl[floorIndex]);

    // u16 wallIndex = gGlobalContext->colCtx.stat.lookupTbl[lookupIndex].wall.head;
    // CitraPrint("wallIndex: %X", wallIndex);
    if (wallIndex != 0xFFFF) ColView_DrawAllFromNode(gGlobalContext->colCtx.stat.polyNodes.tbl[wallIndex]);

    // u16 ceilingIndex = gGlobalContext->colCtx.stat.lookupTbl[lookupIndex].ceiling.head;
    // CitraPrint("ceilingIndex: %X", ceilingIndex);
    if (ceilingIndex != 0xFFFF) ColView_DrawAllFromNode(gGlobalContext->colCtx.stat.polyNodes.tbl[ceilingIndex]);

    // CitraPrint("subdivCount: %X   lookupIndex: %X", subdivCount, lookupIndex);
}

// from wall detection
void ColView_FindStaticLookup(Actor* actor, Vec3i* sector) {
    // if (!isInGame() || actor != &PLAYER->actor) {
    //     return;
    // }
    // CitraPrint("%X, %X, %X", sector->x, sector->y, sector->z);

    // StaticCollisionContext ctx = gGlobalContext->colCtx.stat;
    // s32 lookupId =
    //     sector->x +
    //     (sector->y * ctx.subdivAmount.x) +
    //     (sector->z * ctx.subdivAmount.x * ctx.subdivAmount.y);
    // CitraPrint("lookupId: %X", lookupId);
    // lookupIndex = lookupId;
}

// from floor detection
void ColView_FindStaticLookup2(StaticLookup* lookup, Actor* actor, Vec3f* checkPos) {
    if (!isInGame() || actor != &PLAYER->actor) {
        return;
    }
    ColView_Lookup = lookup;
    // CitraPrint("%X Y:%f", lookup, checkPos->y);
    // if (ColView_Lookup == 0) {
    //     ColView_Lookup = lookup;
    //     CitraPrint("___ %X", lookup);
    // }
    // sceneId = gGlobalContext->sceneNum;
}
