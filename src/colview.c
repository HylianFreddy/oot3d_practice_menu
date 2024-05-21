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
        .color = {
            .r = 1.0f,
            .g = 0.0f,
            .b = 1.0f,
            .a = 0.75f,
        },
    };
}

ColViewPoly getColPolyData(u32 id) {
    CollisionPoly* colPoly = &gGlobalContext->colCtx.stat.colHeader->polyList[id];
    Vec3s* vtxList        = gGlobalContext->colCtx.stat.colHeader->vtxList;

    // CitraPrint("");
    // if (rInputCtx.cur.zr) {
    //     memoryEditorAddress = (u32)colPoly;
    //     menuOpen = true;
    //     menuShow(&gz3DMenu);
    // }

    CitraPrint("poly vtx A= %X", colPoly->vtxData[0]);
    CitraPrint("poly vtx B= %X", colPoly->vtxData[1]);
    CitraPrint("poly vtx C= %X", colPoly->vtxData[2]);
    CitraPrint("poly A X = %X", vtxList[colPoly->vtxData[0]].x);
    CitraPrint("poly A Y = %X", vtxList[colPoly->vtxData[0]].y);
    CitraPrint("poly A Z = %X", vtxList[colPoly->vtxData[0]].z);
    CitraPrint("numVertices = %X", gGlobalContext->colCtx.stat.colHeader->numVertices);
    memoryEditorAddress = (u32)gGlobalContext->colCtx.stat.colHeader;
    menuOpen = 1;
    menuShow(&gz3DMenu);

    return (ColViewPoly){
        .vA = {
            .x=(f32)(vtxList[colPoly->vtxData[0]].x),
            .y=(f32)(vtxList[colPoly->vtxData[0]].y),
            .z=(f32)(vtxList[colPoly->vtxData[0]].z),
        },
        .vB = {
            .x=(f32)(vtxList[colPoly->vtxData[1]].x),
            .y=(f32)(vtxList[colPoly->vtxData[1]].y),
            .z=(f32)(vtxList[colPoly->vtxData[1]].z),
        },
        .vC = {
            .x=(f32)(vtxList[colPoly->vtxData[2]].x),
            .y=(f32)(vtxList[colPoly->vtxData[2]].y),
            .z=(f32)(vtxList[colPoly->vtxData[2]].z),
        },
        .norm = colPoly->norm,
        .color = {
            .r = 1.0f,
            .g = 1.0f,
            .b = 1.0f,
            .a = 0.50f,
        },
    };
}

ColViewPoly getPlayerFloorPoly(void) {
    CollisionPoly* colpoly = PLAYER->actor.floorPoly;

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
    if (rInputCtx.cur.zr) {
        return;
    }

    ColViewPoly dummyPoly = createDummyPoly();
    ColView_DrawPoly(dummyPoly);

    if (PLAYER->actor.floorPoly != 0) {
        // ColView_DrawPoly(getPlayerFloorPoly());
    }

    u16 lookupIndex = 1;
    u16 floorIndex = gGlobalContext->colCtx.stat.lookupTbl[lookupIndex].floor.head;
    SSNode node = gGlobalContext->colCtx.stat.polyNodes.tbl[floorIndex];

    u16 i = 0;
    while (node.next != 0xFFFF) {
        if (i == 2) {
            ColViewPoly poly = getColPolyData(node.polyId);
            ColView_DrawPoly(poly);
        }
        node = gGlobalContext->colCtx.stat.polyNodes.tbl[node.next];
        i++;
    }

    // for (u32 i = 0; i < 100; i++) {
    //     ColView_DrawPoly(getColPolyData(i));
    // }

    // if (ABS(gGlobalContext->cameraPtrs[gGlobalContext->activeCamera]->camDir.y - dummyPoly.norm.y) > 0x4000) {
    // }
}
