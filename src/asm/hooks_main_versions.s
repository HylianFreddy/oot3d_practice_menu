.arm
.text

@ Place hooks in this section to exclude them from KOR and TWN builds
.section .asm_hooks.main_versions_only

.global hook_PlaySound
hook_PlaySound:
    push {r1-r12, lr}
    bl Cheats_RemoveBGM
    pop {r1-r12, lr}
    push {r3-r7, lr}
    b hookReturn_PlaySound

.global hook_SetBGMEntrance
hook_SetBGMEntrance:
    push {r1-r12, lr}
    bl Cheats_RemoveBGM
    pop {r1-r12, lr}
    push {r4-r6, lr}
    b hookReturn_SetBGMEntrance

.global hook_SetBGMDayNight
hook_SetBGMDayNight:
    push {r1-r12, lr}
    bl Cheats_RemoveBGM
    pop {r1-r12, lr}
    push {r4-r6, lr}
    b hookReturn_SetBGMDayNight

.global hook_SetBGMEvent
hook_SetBGMEvent:
    push {r0, r2-r12, lr}
    cpy r0,r1
    bl Cheats_RemoveBGM
    cpy r1,r0
    pop {r0, r2-r12, lr}
    push {r4-r11, lr}
    b hookReturn_SetBGMEvent

.global hook_InstantTextFirstLine
hook_InstantTextFirstLine:
    cmp r9,#0x0
    bgt NoInstantText
    push {r0-r12, lr}
    bl Cheats_IsInstantText
    cmp r0,#0x1
    pop {r0-r12, lr}
    bne NoInstantText
    push {r0-r12, lr}
    ldr r0,[r5,#0x0]
    ldr r1,[r0,#0x20]
    cpy r0,r5
    blx r1
    strb r11,[r4,#0x24]
    pop {r0-r12, lr}
NoInstantText:
    cmp r10,#0xFF
    bx lr

.global hook_InstantTextBoxBreak
hook_InstantTextBoxBreak:
    push {r0-r12, lr}
    bl Cheats_IsInstantText
    cmp r0,#0x1
    pop {r0-r12, lr}
    bne hookReturn_InstantTextBoxBreak
    push {r0-r12, lr}
    ldr r0,[r5,#0x0]
    ldr r1,[r0,#0x20]
    cpy r0,r5
    blx r1
    strb r11,[r4,#0x24]
    pop {r0-r12, lr}
    bne hookReturn_InstantTextBoxBreak

.global hook_InstantTextRemoveOff
hook_InstantTextRemoveOff:
    push {r0-r12, lr}
    bl Cheats_IsInstantText
    cmp r0,#0x1
    pop {r0-r12, lr}
    beq InstantText_SkipControlCode
    ldr r0,[r5,#0x0]
    b hookReturn_InstantTextRemoveOff

.global hook_TurboTextAdvance
hook_TurboTextAdvance:
    push {r0-r12, lr}
    bl Cheats_IsTurboText
    cmp r0,#0x0
    pop {r0-r12, lr}
    cmpeq r0,#0x0
    bx lr

.global hook_TurboTextClose
hook_TurboTextClose:
    push {r0-r12, lr}
    bl Cheats_IsTurboText
    cmp r0,#0x0
    pop {r0-r12, lr}
    cmpeq r0,#0x0
    bx lr

.global hook_TurboTextSignalNPC
hook_TurboTextSignalNPC:
    movne r4,#0x1
    push {r0-r12, lr}
    bl Cheats_IsTurboText
    cmp r0,#0x0
    pop {r0-r12, lr}
    movne r4,#0x1
    bx lr

.global hook_ItemUsability_AnyArea
hook_ItemUsability_AnyArea:
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

.global hook_ItemUsability_AnyAction
hook_ItemUsability_AnyAction:
    push {r0-r12, lr}
    bl Cheats_ForceUsableItems
    pop {r0-r12, lr}
    cmp r7,#0x0
    bx lr

.global hook_Gfx_SleepQueryCallback
hook_Gfx_SleepQueryCallback:
    push {r0-r12, lr}
    bl Gfx_SleepQueryCallback
    pop {r0-r12, lr}
    add r0,r0,#0x9C
    b hookReturn_SleepQueryCallback

.global hook_OverrideSceneSetup
hook_OverrideSceneSetup:
    push {r0-r12, lr}
    bl Warps_OverrideSceneSetup
    pop {r0-r12, lr}
    mov r0,#0x2
    bx lr

.global hook_LoadGame
hook_LoadGame:
    add r0, r5, r4
    push {r0-r12, lr}
    bl Settings_LoadGame
    pop {r0-r12, lr}
    bx lr

.global hook_CameraUpdate
hook_CameraUpdate:
    push {r0-r12,lr}
    bl Camera_OnCameraUpdate
    cmp r0,#0x0
    pop {r0-r12,lr}
    cpyeq r6,r0
    bxeq lr
    ldmia sp!,{r4-r11,pc}

.global hook_BlackScreenFix
hook_BlackScreenFix:
    cmp r0,#0x0 @ cutscene pointer, if 0 the fade-in will start
    bxeq lr
    push {r0-r12, lr}
    bl Cheats_ShouldFixBlackScreen
    cmp r0,#0x1
    pop {r0-r12, lr}
    bx lr
