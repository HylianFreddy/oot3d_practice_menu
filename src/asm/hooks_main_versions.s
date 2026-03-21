.arm
.text

.macro HOOK name
    .global hook_\name
hook_\name:
.endm

@ Place hooks in this section to exclude them from KOR and TWN builds
.section .asm_hooks.main_versions_only

HOOK PlaySound
    push {r1-r12, lr}
    bl Cheats_RemoveBGM
    pop {r1-r12, lr}
    push {r3-r7, lr}
    b hookReturn_PlaySound

HOOK SetBGMEntrance
    push {r1-r12, lr}
    bl Cheats_RemoveBGM
    pop {r1-r12, lr}
    push {r4-r6, lr}
    b hookReturn_SetBGMEntrance

HOOK SetBGMDayNight
    push {r1-r12, lr}
    bl Cheats_RemoveBGM
    pop {r1-r12, lr}
    push {r4-r6, lr}
    b hookReturn_SetBGMDayNight

HOOK SetBGMEvent
    push {r0, r2-r12, lr}
    cpy r0,r1
    bl Cheats_RemoveBGM
    cpy r1,r0
    pop {r0, r2-r12, lr}
    push {r4-r11, lr}
    b hookReturn_SetBGMEvent

HOOK ParseTextCharacter
    cpy r0,r6
    push {r0-r12, lr}
    cpy r0,r5 @ Text Object
    @ r1: nextChars
    cpy r2,r9 @ Char Index (loop counter)
    bl Message_ApplyInstantText
    pop {r0-r12, lr}
    bx lr

HOOK ParseTextControlCode
    ldrb r0,[r6,#0x4] @ Control Code identifier
    push {r0-r12, lr}
    bl Message_ShouldSkipTextControlCode
    cmp r0,#0x0
    pop {r0-r12, lr}
    addne lr,lr,#0x8 @ Skip control code
    bx lr

HOOK TurboTextAdvance
    push {r0-r12, lr}
    bl Message_ShouldAdvanceTurboText
    cmp r0,#0x0
    pop {r0-r12, lr}
    cmpeq r0,#0x0
    bx lr

HOOK TurboTextClose
    push {r0-r12, lr}
    bl Message_ShouldAdvanceTurboText
    cmp r0,#0x0
    pop {r0-r12, lr}
    cmpeq r0,#0x0
    bx lr

HOOK TurboTextSignalNPC
    movne r4,#0x1
    push {r0-r12, lr}
    bl Message_ShouldAdvanceTurboText
    cmp r0,#0x0
    pop {r0-r12, lr}
    movne r4,#0x1
    bx lr

HOOK ItemUsability_AnyArea
@ R11 is InterfaceContext, used after this hook
@ only to get the restriction flags.
@ If the cheat is active, the function will
@ return an array of empty restriction flags.
@ Forge R11 so that when the game tries to access
@ the flags, it finds the empty array instead.
@ Don't store R0 because it's overwritten below anyway.
    push {r1-r12, lr}
    bl Cheats_GetFakeItemRestrictions
    cmp r0,#0x0
    pop {r1-r12, lr}
    subne r0,r0,#0x200
    subne r11,r0,#0x9E
    ldrb r0,[r11,#0x29F]
    bx lr

HOOK ItemUsability_AnyAction
    push {r0-r12, lr}
    bl Cheats_ForceUsableItems
    pop {r0-r12, lr}
    cmp r7,#0x0
    bx lr

HOOK Gfx_SleepQueryCallback
    push {r0-r12, lr}
    bl Gfx_SleepQueryCallback
    pop {r0-r12, lr}
    add r0,r0,#0x9C
    b hookReturn_SleepQueryCallback

HOOK OverrideSceneSetup
    push {r0-r12, lr}
    bl Warps_OverrideSceneSetup
    pop {r0-r12, lr}
    mov r0,#0x2
    bx lr

HOOK LoadGame
    add r0, r5, r4
    push {r0-r12, lr}
    bl Settings_LoadGame
    pop {r0-r12, lr}
    bx lr

HOOK CameraUpdate
    push {r0-r12,lr}
    bl Camera_OnCameraUpdate
    cmp r0,#0x0
    pop {r0-r12,lr}
    cpyeq r6,r0
    bxeq lr
    ldmia sp!,{r4-r11,pc}

HOOK BlackScreenFix
    cmp r0,#0x0 @ cutscene pointer, if 0 the fade-in will start
    bxeq lr
    push {r0-r12, lr}
    bl Cheats_ShouldFixBlackScreen
    cmp r0,#0x1
    pop {r0-r12, lr}
    bx lr
