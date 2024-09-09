#include "menus/settings.h"
#include "menus/watches.h"
#include "3ds/extdata.h"
#include "common.h"
#include <string.h>

ExtSaveData gExtSaveData;
u8 selectedProfile = 0;

Menu SettingsMenu = {
    "Settings",
    .nbItems = NUMBER_OF_SETTING_OPTIONS,
    .initialCursorPos = 0,
    {
        {"Profile: 0", METHOD, .method = Settings_CycleProfile},
        {"Save Profile", METHOD, .method = Settings_SaveExtSaveData},
        {"Load Profile", METHOD, .method = Settings_LoadExtSaveData},
        {"Other Toggle Settings (Not Saved)", METHOD, .method = Settings_ShowToggleSettingsMenu},
    }
};

ToggleMenu ToggleSettingsMenu = {
    "Other Toggle Settings (Not Saved)",
    .nbItems = NUMBER_OF_TOGGLE_SETTINGS,
    .initialCursorPos = 0,
    {
        {1, "Pause/Commands Display", METHOD, .method = Settings_Toggle},
        {1, "Remember cursor position in all menus", METHOD, .method = Settings_Toggle},
        {1, "Update watch addresses at profile load", METHOD, .method = Settings_Toggle},
        {1, "Game ignores last command button", METHOD, .method = Settings_Toggle},
        {1, "Main ASM hook (disable to test lag,\n    most features will stop working)", METHOD, .method = Settings_Toggle},
    }
};

void Settings_ShowToggleSettingsMenu(void){
    ToggleMenuShow(&ToggleSettingsMenu);
}

void Settings_Toggle(s32 selected) {
    ToggleSettingsMenu.items[selected].on = !ToggleSettingsMenu.items[selected].on;
    setAlert("", 0);
}

void Settings_CycleProfile(void) {
    selectedProfile++;
    selectedProfile %= 3;
    SettingsMenu.items[SETTINGS_PROFILE].title[9] = selectedProfile + '0';
}

void Settings_InitExtSaveData(void) {
    memset(&gExtSaveData, 0, sizeof(gExtSaveData));
    gExtSaveData.version = EXTSAVEDATA_VERSION; // Do not change this line
    memcpy(gExtSaveData.cheats, cheats, sizeof(cheats));
    gExtSaveData.cheatForcedUsableItems = forcedUsableItems;
    memcpy(gExtSaveData.commands, commandList, sizeof(commandList));
    memcpy(gExtSaveData.watches, watches, sizeof(watches));
    gExtSaveData.info.memAddrs.globalCtx = gGlobalContext;
    gExtSaveData.info.memAddrs.actorHeap = gActorHeapAddress;
    gExtSaveData.info.region = REGION;
}

// copies saved values to the menu structs
void Settings_ApplyExtSaveData(void) {
    memcpy(cheats, gExtSaveData.cheats, sizeof(cheats));
    forcedUsableItems = gExtSaveData.cheatForcedUsableItems;
    for (u32 i = 0; i < NUMBER_OF_COMMANDS; i++) {
        commandList[i].comboLen = gExtSaveData.commands[i].comboLen;
        memcpy(commandList[i].inputs, gExtSaveData.commands[i].inputs, sizeof(gExtSaveData.commands[i].inputs));
        commandList[i].strict = gExtSaveData.commands[i].strict;
    }
    commandInit = 1;
    memcpy(watches, gExtSaveData.watches, sizeof(watches));
}

void Settings_SaveExtSaveData(void) {
    Settings_InitExtSaveData();

    char path[] = "/gz3D_X.bin";

    Result res;
    FS_Archive fsa;

    if(!R_SUCCEEDED(res = extDataMount(&fsa))) {
        setAlert("Failed to save! ", 90);
        return;
    }

    path[6] = selectedProfile + '0';

    extDataWriteFileDirectly(fsa, path, &gExtSaveData, 0, sizeof(gExtSaveData));

    extDataUnmount(fsa);

    char* alert = "Profile X saved";
    alert[8] = selectedProfile + '0';
    setAlert(alert, 90);
}

void Settings_LoadExtSaveData(void) {
    char path[] = "/gz3D_X.bin";
    u32 version;

    Result res;
    FS_Archive fsa;
    Handle fileHandle;

    if (R_FAILED(res = extDataMount(&fsa))) {
        setAlert("Failed to load! ", 90);
        return;
    }

    path[6] = selectedProfile + '0';

    if (R_FAILED(res = extDataOpen(&fileHandle, fsa, path))) {
        extDataUnmount(fsa);
        setAlert("Failed to load! ", 90);
        return;
    }

    extDataReadFile(fileHandle, &version, 0, sizeof(version));
    if (version != EXTSAVEDATA_VERSION) {
        extDataClose(fileHandle);
        extDataUnmount(fsa);
        setAlert("Failed to load! ", 90);
        return;
    }

    extDataReadFile(fileHandle, &gExtSaveData, 0, sizeof(gExtSaveData));

    extDataClose(fileHandle);
    extDataUnmount(fsa);

    Settings_ApplyExtSaveData();

    if (gInit && ToggleSettingsMenu.items[TOGGLESETTINGS_UPDATE_WATCHES].on) {
        Settings_UpdateWatchAddresses();
    }

    char* alert = "Profile X loaded";
    alert[8] = selectedProfile + '0';
    setAlert(alert, 90);
}

void Settings_UpdateWatchAddresses(void) {
    if (gExtSaveData.info.region == REGION_UNDEFINED) {
        setAlert("Cannot verify watches", 90);
        return;
    }
    bool updated = false;
    s32 actorHeapOffset = gActorHeapAddress - gExtSaveData.info.memAddrs.actorHeap;
    s32 globalCtxOffset = (void*)gGlobalContext - gExtSaveData.info.memAddrs.globalCtx;
    for (int i = 0; i < WATCHES_MAX; i++) {
        #define watchAddr watches[i].addr
        s32 offset = 0;
        if (watchAddr >= gExtSaveData.info.memAddrs.actorHeap) {
            offset = actorHeapOffset;
        }
        else if (watchAddr >= gExtSaveData.info.memAddrs.globalCtx) {
            offset = globalCtxOffset;
        }
        else if (gExtSaveData.info.region == REGION_EUR && REGION == REGION_USA) {
            if (watchAddr >= (void*)0x41A168 && watchAddr <= (void*)0x4366AF) {
                offset = -0x24;
            } else if (watchAddr >= (void*)0x4366B0 && watchAddr <= (void*)0x4A5AFF) {
                offset = -0x20;
            }
        }
        else if (gExtSaveData.info.region == REGION_USA && REGION == REGION_EUR) {
            if (watchAddr >= (void*)0x41A144 && watchAddr <= (void*)0x43668B) {
                offset = 0x24;
            } else if (watchAddr >= (void*)0x436690 && watchAddr <= (void*)0x4A5ADF) {
                offset = 0x20;
            }
        }

        if (offset != 0) {
            watchAddr += offset;
            updated = true;
        }
        #undef watchAddr
    }

    if (updated) {
        setAlert("Watches updated", 90);
    }
}

// This function is called when loading a save file.
// In order to save the settings, the extdata need to already be present. Normally the
// extDataMount function (used when saving/loading extdata) creates them if they don't exist,
// but for some reason it fails to do so when it's called from the menu loop. The data creation
// only seems to work when saving/loading a save file, so the purpose of this function is to
// make sure the data exist (if the user has played the Randomizer, they would already be present)
void Settings_LoadGame(void) {
    FS_Archive fsa;
    if (R_SUCCEEDED(extDataMount(&fsa))) {
        extDataUnmount(fsa);
    }
}
