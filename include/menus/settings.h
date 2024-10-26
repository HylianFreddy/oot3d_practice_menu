#include "menu.h"
#include "menus/cheats.h"
#include "menus/commands.h"
#include "menus/watches.h"
#include "z3D/z3D.h"

#define EXTSAVEDATA_VERSION 0

#if Version_USA
    #define CURRENT_REGION REGION_USA
#elif Version_EUR
    #define CURRENT_REGION REGION_EUR
#elif Version_JPN
    #define CURRENT_REGION REGION_JPN
#elif Version_KOR
    #define CURRENT_REGION REGION_KOR
#elif Version_TWN
    #define CURRENT_REGION REGION_TWN
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
    REGION_JPN,
    REGION_KOR,
    REGION_TWN,
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
    TOGGLESETTINGS_GAME_IGNORES_COMMAND_BUTTON,
    TOGGLESETTINGS_MAIN_HOOK,
    NUMBER_OF_TOGGLE_SETTINGS,
} ToggleSettings;

typedef enum WatchUpdateResult {
    WATCHUPDATE_NONE,
    WATCHUPDATE_SUCCESS,
    WATCHUPDATE_FAILED,
} WatchUpdateResult;

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
    char     padding[4];
    Command  commands[32];
    Watch    watches[30];
    ExtInfo  info;
    u8       settings[12];
} ExtSaveData;
_Static_assert(sizeof(ExtSaveData) == 0x8A8, "ExtSaveData size");
// Non-breaking changes to this struct:
// - last 2 unused Watch slots remapped to "info" and "settings"

extern ExtSaveData gExtSaveData;
