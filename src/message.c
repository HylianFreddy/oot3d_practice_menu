#include "z3D/z3D.h"
#include "message.h"
#include "input.h"
#include "menus/cheats.h"

u32 Message_ShouldAdvanceTurboText(void) {
    return cheats[CHEATS_QUICK_TEXT] && rInputCtx.cur.b;
}

u32 Message_ShouldSkipTextControlCode(TextControlCode ctrl) {
    // Make text skippable or instant depending on setting.
    if (cheats[CHEATS_QUICK_TEXT] && (ctrl == TEXT_CTRL_UNSKIPPABLE || ctrl == TEXT_CTRL_INSTANT_TEXT_OFF)) {
        return TRUE; // Bypass control code
    }

    return FALSE; // Resume base game function
}

void Message_ApplyInstantText(void* textObj, char* nextChars, u32 charIdx) {
    if (!cheats[CHEATS_QUICK_TEXT]) {
        return;
    }

    // Set instant text when parsing the first character of the text or immediately after a box break.
    u8 isFirstChar              = charIdx == 0;
    u8 isAfterBoxBreak8BitChar  = charIdx >= 2 && nextChars[-2] == '\x7F' && nextChars[-1] == TEXT_CTRL_WAIT_FOR_INPUT;
    u8 isAfterBoxBreak16BitChar = charIdx >= 4 && nextChars[-4] == '\x7F' && nextChars[-2] == TEXT_CTRL_WAIT_FOR_INPUT;
    if (isFirstChar || isAfterBoxBreak8BitChar || isAfterBoxBreak16BitChar) {
        Message_SetInstantText(textObj);
    }
}
