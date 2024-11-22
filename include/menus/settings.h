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

extern ToggleMenu SettingsMenu;
extern Menu ProfilesMenu;
#define SETTING_ENABLED(opt) (SettingsMenu.items[opt].on)

extern u8 selectedProfile;

void Settings_CycleProfile(void);
void Settings_InitExtSaveData(void);
void Settings_SaveExtSaveData(void);
void Settings_LoadExtSaveData(void);
void Settings_Toggle(s32 selected);
void Settings_ShowSettingsMenu(void);
void Settings_UpdateWatchAddresses(void);

enum Region {
    REGION_UNDEFINED,
    REGION_USA,
    REGION_EUR,
    REGION_JPN,
    REGION_KOR,
    REGION_TWN,
};

enum SettingsEnum {
    SETTINGS_HIDE_ALERT_DISPLAY,
    SETTINGS_RESET_CURSOR,
    SETTINGS_ALT_TITLE_COLOR,
    SETTINGS_MAX,
};

enum ProfilesEnum {
    PROFILES_NUMBER,
    PROFILES_SAVE,
    PROFILES_LOAD,
    PROFILES_MAX,
};

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
