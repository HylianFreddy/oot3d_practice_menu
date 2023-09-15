#include "menu.h"
#include "menus/cheats.h"
#include "menus/commands.h"
#include "menus/watches.h"
#include "z3D/z3D.h"

#define EXTSAVEDATA_VERSION 0

// Redefine REGION variable from Makefile
#undef REGION
#ifdef Version_JP
    #define REGION REGION_JP
#elif Version_EUR
    #define REGION REGION_EUR
#else // Version_USA
    #define REGION REGION_USA
#endif

extern Menu SettingsMenu;
extern ToggleMenu ToggleSettingsMenu;

extern u8 selectedProfile;

void Settings_CycleProfile(void);
void Settings_InitExtSaveData(void);
void Settings_SaveExtSaveData(void);
void Settings_LoadExtSaveData(void);
void Settings_Toggle(s32 selected);
void Settings_ShowToggleSettingsMenu(void);
void Settings_UpdateWatchAddresses(void);

enum Region {
    REGION_UNDEFINED,
    REGION_USA,
    REGION_EUR,
    REGION_JP,
};

typedef enum {
    SETTINGS_PROFILE,
    SETTINGS_SAVE,
    SETTINGS_LOAD,
    SETTINGS_TOGGLES,
    NUMBER_OF_SETTING_OPTIONS,
} Settings;

typedef enum {
    TOGGLESETTINGS_PAUSE_AND_COMMANDS_DISPLAY,
    TOGGLESETTINGS_REMEMBER_CURSOR_POSITION,
    TOGGLESETTINGS_UPDATE_WATCHES,
    TOGGLESETTINGS_MAIN_HOOK,
    NUMBER_OF_TOGGLE_SETTINGS,
} ToggleSettings;

typedef struct MemAddrs {
    void* globalCtx;
    void* actorHeap;
    void* unused[4];
} MemAddrs;

typedef struct ExtInfo {
    MemAddrs memAddrs;
    u8       region;
    u8       unused[11];
} ExtInfo;

typedef struct {
    u32      version;
    u8       cheats[32];
    u8       cheatForcedUsableItems;
    char     padding[3];
    Command  commands[32];
    Watch    watches[30];
    ExtInfo  info;
    u8       settings[12];
} ExtSaveData;
_Static_assert(sizeof(ExtSaveData) == 0x8A8, "ExtSaveData size");
// Non-breaking changes to this struct:
// - last 2 unused Watch slots remapped to "info" and "settings"

extern ExtSaveData gExtSaveData;
