#include "menu.h"
#include "menus/equips.h"
#include "draw.h"
#include "input.h"
#include "z3D/z3D.h"

void Equips_EquipsMenuInit(void){
    for(u8 i = 0; i < 5; i++) {
        EquipsMenu.items[i].amount = gSaveContext.equips.buttonItems[i];
    }
    for(u8 i = 0; i < 4; i++) {
        EquipsMenu.items[i+5].amount = gSaveContext.equips.buttonSlots[i];
    }
    for(u8 i = 0; i < 4; i++) {
        EquipsMenu.items[EQUIP_SWORD + i].amount = (gSaveContext.equips.equipment >> i*4) & 0x3;
    }

    EquipsMenu.items[EQUIP_TEMP_B].amount = gSaveContext.buttonStatus[0];

    if(gSaveContext.infTable[29] &= 0x1) {
        EquipsMenu.items[EQUIP_SWORD].title = "Sword (0-3) - Swordless Flag ON ";
    } else {
        EquipsMenu.items[EQUIP_SWORD].title = "Sword (0-3) - Swordless Flag OFF";
    }
}

void Equips_ShowEquipsMenu(void){
    Equips_EquipsMenuInit();
    AmountMenuShow(&EquipsMenu);
}

void Equips_ModifyButton(s32 selected){
    if(selected < 5) {
        gSaveContext.equips.buttonItems[selected] = EquipsMenu.items[selected].amount;
    } else {
        gSaveContext.equips.buttonSlots[selected - 5] = EquipsMenu.items[selected].amount;
    }
}

void Equips_ModifyTempB(s32 selected){
    gSaveContext.buttonStatus[0] = EquipsMenu.items[selected].amount;
}

void Equips_ModifyGear(s32 selected){
    u32 sword = EquipsMenu.items[EQUIP_SWORD].amount;
    u32 shield = EquipsMenu.items[EQUIP_SHIELD].amount;
    u32 tunic = EquipsMenu.items[EQUIP_TUNIC].amount;
    u32 boots = EquipsMenu.items[EQUIP_BOOTS].amount;
    gSaveContext.equips.equipment = ((sword < 4) ? sword : 0) +
                                    (((shield < 4) ? shield : 0) << 4) +
                                    (((tunic < 4) ? tunic : 0) << 8) +
                                    (((boots < 4) ? boots : 0) << 12);

    if(selected == EQUIP_SWORD) {
        switch (EquipsMenu.items[EQUIP_SWORD].amount) {
            case (0) : gSaveContext.equips.buttonItems[0] = 255; break;
            case (1) : gSaveContext.equips.buttonItems[0] =  59; break;
            case (2) : gSaveContext.equips.buttonItems[0] =  60; break;
            case (3) : gSaveContext.equips.buttonItems[0] =  61; break;
        }

        if(EquipsMenu.items[EQUIP_SWORD].amount == 0 || ADDITIONAL_FLAG_BUTTON) {
            gSaveContext.infTable[29] |= 0x1; //set swordless flag
            EquipsMenu.items[EQUIP_SWORD].title = "Sword (0-3) - Swordless Flag ON ";
        } else {
            gSaveContext.infTable[29] &= ~0x1; //clear swordless flag
            EquipsMenu.items[EQUIP_SWORD].title = "Sword (0-3) - Swordless Flag OFF";
        }
    }

#if !Version_KOR && !Version_TWN
    Player_SetEquipmentData(gGlobalContext, PLAYER);
#endif

    Equips_EquipsMenuInit();
}

AmountMenu EquipsMenu = {
    "Equips",
    .nbItems = 14,
    .initialCursorPos = 0,
    {
        {.amount = 0, .isSigned = false, .min = 0, .max = 255, .nDigits = 3, .hex = false,
            .title = "B button item", .method = Equips_ModifyButton},
        {.amount = 0, .isSigned = false, .min = 0, .max = 255, .nDigits = 3, .hex = false,
            .title = "Y button item", .method = Equips_ModifyButton},
        {.amount = 0, .isSigned = false, .min = 0, .max = 255, .nDigits = 3, .hex = false,
            .title = "X button item", .method = Equips_ModifyButton},
        {.amount = 0, .isSigned = false, .min = 0, .max = 255, .nDigits = 3, .hex = false,
            .title = "I button item", .method = Equips_ModifyButton},
        {.amount = 0, .isSigned = false, .min = 0, .max = 255, .nDigits = 3, .hex = false,
            .title = "II button item", .method = Equips_ModifyButton},
        {.amount = 0, .isSigned = false, .min = 0, .max = 255, .nDigits = 3, .hex = false,
            .title = "Y button slot", .method = Equips_ModifyButton},
        {.amount = 0, .isSigned = false, .min = 0, .max = 255, .nDigits = 3, .hex = false,
            .title = "X button slot", .method = Equips_ModifyButton},
        {.amount = 0, .isSigned = false, .min = 0, .max = 255, .nDigits = 3, .hex = false,
            .title = "I button slot", .method = Equips_ModifyButton},
        {.amount = 0, .isSigned = false, .min = 0, .max = 255, .nDigits = 3, .hex = false,
            .title = "II button slot", .method = Equips_ModifyButton},
        {.amount = 0, .isSigned = false, .min = 0, .max =   3, .nDigits = 1, .hex = false,
            .title = "Sword (0-3) - Swordless Flag OFF", .method = Equips_ModifyGear},
        {.amount = 0, .isSigned = false, .min = 0, .max =   3, .nDigits = 1, .hex = false,
            .title = "Shield (0-3)", .method = Equips_ModifyGear},
        {.amount = 0, .isSigned = false, .min = 0, .max =   3, .nDigits = 1, .hex = false,
            .title = "Tunic (0-3)", .method = Equips_ModifyGear},
        {.amount = 0, .isSigned = false, .min = 0, .max =   3, .nDigits = 1, .hex = false,
            .title = "Boots (0-3)", .method = Equips_ModifyGear},
        {.amount = 0, .isSigned = false, .min = 0, .max = 255, .nDigits = 3, .hex = false,
            .title = "Temp B", .method = Equips_ModifyTempB},
    }
};
