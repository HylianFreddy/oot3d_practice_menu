#include "menu.h"
#include "menus/cheats.h"
#include "menus/commands.h"
#include "menus/watches.h"
#include "z3D/z3D.h"

#define EXTSAVEDATA_VERSION 0

extern Menu SettingsMenu;
extern ToggleMenu ToggleSettingsMenu;

extern u8 selectedProfile;

void Settings_ShowSettingsMenu(void);
void Settings_CycleProfile(void);
void Settings_InitExtSaveData(void);
void Settings_SaveExtSaveData(void);
void Settings_LoadExtSaveData(void);
void Settings_Toggle(s32 selected);
void Settings_ShowToggleSettingsMenu(void);

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
    TOGGLESETTINGS_MAIN_HOOK,
    NUMBER_OF_TOGGLE_SETTINGS,
} ToggleSettings;

typedef struct {
    u32      version;
    u8       cheats[32];
    u8       cheatForcedUsableItems;
    Command  commands[32];
    Watch    watches[32];
} ExtSaveData;

extern ExtSaveData gExtSaveData;
