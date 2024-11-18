#include "input.h"
#include "z3D/z3D.h"
#include "hid.h"
#include "3ds/svc.h"
#include "utils.h"
#include "3ds/types.h"

#define HID_PAD (real_hid.pad.pads[real_hid.pad.index].curr.val)
#define SCROLL_BUTTONS  (DPAD_ANY | CPAD_ANY | BUTTON_A)

InputContext rInputCtx;
u8 scrollDelay = 1;

void Input_Update(void) {
    irrstScanInput();
    u32 zKeys = irrstKeysHeld();

    rInputCtx.cur.val = real_hid.pad.pads[real_hid.pad.index].curr.val;
    rInputCtx.cur.zl = (zKeys >> 14) & 1;
    rInputCtx.cur.zr = (zKeys >> 15) & 1;
    rInputCtx.pressed.val = (rInputCtx.cur.val) & (~rInputCtx.old.val);
    rInputCtx.up.val = (~rInputCtx.cur.val) & (rInputCtx.old.val);
    rInputCtx.old.val = rInputCtx.cur.val;

    rInputCtx.touchX       = real_hid.touch.touches[real_hid.touch.index].touch.x;
    rInputCtx.touchY       = real_hid.touch.touches[real_hid.touch.index].touch.y;
    rInputCtx.touchPressed = real_hid.touch.touches[real_hid.touch.index].updated && !rInputCtx.touchHeld;
    rInputCtx.touchHeld    = real_hid.touch.touches[real_hid.touch.index].updated;

    irrstCstickRead(&(rInputCtx.cStick));
}

u32 Input_WaitWithTimeout(u32 msec) {
    u32 n = 0;

    Input_Update(); // Record initial state of buttons to detect changes against

    const bool isScrollButtonPressed = (HID_PAD & SCROLL_BUTTONS) != 0;

    // We special some buttons as we want to automatically scroll the cursor or
    // allow amount editing at a reasonable pace as long as it's held down.
    if (isScrollButtonPressed && scrollDelay) {
        // Wait 250 ms the first time, as long as the button isn't released
        for(u32 i = 0; HID_PAD && i < 250; i+=10) {
            svcSleepThread(10 * 1000 * 1000LL);
        }
        scrollDelay = 0;
        Input_Update(); // If the scroll button has been released, clear it from the current buttons
    }
    else if (isScrollButtonPressed || rInputCtx.touchHeld) {
        // Wait 50 milliseconds while a scroll button or the touch screen is pressed,
        // so that we don't scroll the menu too fast or redraw things too quickly.
        svcSleepThread(50 * 1000 * 1000LL);
    }
    else {
        // Wait for a new button or the touch screen to be pressed.
        while (rInputCtx.pressed.val == 0 && rInputCtx.touchPressed == 0 && (msec == 0 || n <= msec)) {
            scrollDelay = 1;
            Input_Update();
            svcSleepThread(1 * 1000 * 1000LL);
            n++;
        }
    }

    if (msec != 0 && n >= msec) {
        return 0;
    }

    return rInputCtx.cur.val;
}

u32 Input_Wait(void) {
    return Input_WaitWithTimeout(0);
}
