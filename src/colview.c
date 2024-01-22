#include "colview.h"

#define ABS(x) ((x) >= 0 ? (x) : -(x))

static ColViewPoly createDummyPoly(void) {
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
        .color = {
            .r = 1.0f,
            .g = 0.0f,
            .b = 1.0f,
            .a = 0.75f,
        },
    };
}

static ColViewPoly getColPolyData(CollisionPoly colPoly) { //u32 id
    // CollisionPoly colPoly = gGlobalContext->colCtx.stat.colHeader->polyList[id];
    Vec3s* vtxList        = gGlobalContext->colCtx.stat.colHeader->vtxList;

    return (ColViewPoly){
        .vA = {
            .x=(f32)(vtxList[colPoly.vtxData[0]].x),
            .y=(f32)(vtxList[colPoly.vtxData[0]].y),
            .z=(f32)(vtxList[colPoly.vtxData[0]].z),
        },
        .vB = {
            .x=(f32)(vtxList[colPoly.vtxData[1]].x),
            .y=(f32)(vtxList[colPoly.vtxData[1]].y),
            .z=(f32)(vtxList[colPoly.vtxData[1]].z),
        },
        .vC = {
            .x=(f32)(vtxList[colPoly.vtxData[2]].x),
            .y=(f32)(vtxList[colPoly.vtxData[2]].y),
            .z=(f32)(vtxList[colPoly.vtxData[2]].z),
        },
        .norm = colPoly.norm,
        .color = {
            .r = 1.0f,
            .g = 1.0f,
            .b = 1.0f,
            .a = 0.50f,
        },
    };
}

static ColViewPoly getPlayerFloorPoly(void) {
    CollisionPoly* colpoly = PLAYER->actor.floorPoly;

    return getColPolyData(*colpoly);

    return (ColViewPoly){
        .vA = {
            .x=(f32)(gGlobalContext->colCtx.stat.colHeader->vtxList[colpoly->vtxData[0]].x),
            .y=(f32)(gGlobalContext->colCtx.stat.colHeader->vtxList[colpoly->vtxData[0]].y),
            .z=(f32)(gGlobalContext->colCtx.stat.colHeader->vtxList[colpoly->vtxData[0]].z),
        },
        .vB = {
            .x=(f32)(gGlobalContext->colCtx.stat.colHeader->vtxList[colpoly->vtxData[1]].x),
            .y=(f32)(gGlobalContext->colCtx.stat.colHeader->vtxList[colpoly->vtxData[1]].y),
            .z=(f32)(gGlobalContext->colCtx.stat.colHeader->vtxList[colpoly->vtxData[1]].z),
        },
        .vC = {
            .x=(f32)(gGlobalContext->colCtx.stat.colHeader->vtxList[colpoly->vtxData[2]].x),
            .y=(f32)(gGlobalContext->colCtx.stat.colHeader->vtxList[colpoly->vtxData[2]].y),
            .z=(f32)(gGlobalContext->colCtx.stat.colHeader->vtxList[colpoly->vtxData[2]].z),
        },
        .norm = colpoly->norm,
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

void ColView_DrawCollision(void) {
    ColViewPoly dummyPoly = createDummyPoly();
    ColView_DrawPoly(dummyPoly);

    // for (u32 i = 0; i < 10; i++) {
    //     ColView_DrawPoly(getColPolyData(i));
    // }

    if (PLAYER->actor.floorPoly != 0) {
        ColView_DrawPoly(getPlayerFloorPoly());
    }

    // if (ABS(gGlobalContext->cameraPtrs[gGlobalContext->activeCamera]->camDir.y - dummyPoly.norm.y) > 0x4000) {
    // }
}
