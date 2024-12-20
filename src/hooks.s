.arm
.text

.global hook_into_Gfx_Update
hook_into_Gfx_Update:
    push {r0-r12, lr}
    bl advance_main
    pop {r0-r12, lr}
    pop {r4-r8, pc}

.global hook_before_Play_Update
hook_before_Play_Update:
    push {r0-r12, lr}
    bl before_Play_Update
    pop {r0-r12, lr}
.if (_KOR_ || _TWN_)
    cpy r8,r0
.else
    cpy r7,r0
.endif
    bx lr

.global hook_after_Play_Update
hook_after_Play_Update:
    push {lr}
    push {r0-r12}
    bl after_Play_Update
    pop {r0-r12}
    @ Call Play_Draw
.if (_USA_ || _EUR_)
    bl 0x2E25F0
.endif
.if _JPN_
    bl 0x2E2108
.endif
.if _TWN_
    bl 0x2FC1A0
.endif
.if _KOR_
    bl 0x2FC0A0
.endif
    push {r0-r12}
    bl after_Play_Draw
    pop {r0-r12}
    pop {pc}

.global hook_PlaySound
hook_PlaySound:
    push {r1-r12, lr}
    bl Cheats_RemoveBGM
    pop {r1-r12, lr}
    push {r3-r7, lr}
.if _JPN_
    b 0x35C044
.else
    b 0x35C52C
.endif

.global hook_SetBGMEntrance
hook_SetBGMEntrance:
    push {r1-r12, lr}
    bl Cheats_RemoveBGM
    pop {r1-r12, lr}
    push {r4-r6, lr}
.if _JPN_
    b 0x330B64
.else
    b 0x33104C
.endif

.global hook_SetBGMDayNight
hook_SetBGMDayNight:
    push {r1-r12, lr}
    bl Cheats_RemoveBGM
    pop {r1-r12, lr}
    push {r4-r6, lr}
.if _USA_
    b 0x483C8C
.endif
.if _EUR_
    b 0x483CAC
.endif
.if _JPN_
    b 0x483C64
.endif

.global hook_SetBGMEvent
hook_SetBGMEvent:
    push {r0, r2-r12, lr}
    cpy r0,r1
    bl Cheats_RemoveBGM
    cpy r1,r0
    pop {r0, r2-r12, lr}
    push {r4-r11, lr}
.if _JPN_
    b 0x36E75C
.else
    b 0x36EC44
.endif

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
.if _JPN_
    bne 0x2DFE60
.else
    bne 0x2E0EE0
.endif
    push {r0-r12, lr}
    ldr r0,[r5,#0x0]
    ldr r1,[r0,#0x20]
    cpy r0,r5
    blx r1
    strb r11,[r4,#0x24]
    pop {r0-r12, lr}
.if _JPN_
    bne 0x2DFE60
.else
    bne 0x2E0EE0
.endif

.global hook_InstantTextRemoveOff
hook_InstantTextRemoveOff:
    push {r0-r12, lr}
    bl Cheats_IsInstantText
    cmp r0,#0x1
    pop {r0-r12, lr}
.if _JPN_
    beq 0x2DFE54
    ldr r0,[r5,#0x0]
    b 0x2DF64C
.else
    beq 0x2E0ED4
    ldr r0,[r5,#0x0]
    b 0x2E06CC
.endif

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
    b 0x3FD6C8

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

.global hook_HaltActors
hook_HaltActors:
    push {r0-r12,lr}
    bl Scene_HaltActorsEnabled
    cmp r0,#0x0
    pop {r0-r12,lr}
.if (_USA_ || _EUR_)
    bne 0x2E4090
.endif
.if (_JPN_)
    bne 0x2E3BA8
.endif
.if (_KOR_)
    bne 0x2FDD08
.endif
.if (_TWN_)
    bne 0x2FDE08
.endif
    cmp r0,#0x0
    bx lr

.global hook_before_GameState_Loop
hook_before_GameState_Loop:
.if (_KOR_ || _TWN_)
    push {r0-r12, lr}
    cpy r0,r4
    bl before_GameState_Loop
    pop {r0-r12, lr}
    cpy r0,r9
    bx lr
.else
    push {r0-r12, lr}
    cpy r0,r5
    bl before_GameState_Loop
    pop {r0-r12, lr}
    cpy r0,r4
    bx lr
.endif

.global hook_after_GameState_Update
hook_after_GameState_Update:
    push {r0-r12, lr}
    bl checkFastForward
    cmp r0,#0x0
    pop {r0-r12, lr}
.if (_KOR_ || _TWN_)
    beq 0x102880
.else
    beq 0x418B88 @ handles drawing screen
.endif
    bx lr

.global hook_BlackScreenFix
hook_BlackScreenFix:
    cmp r0,#0x0 @ cutscene pointer, if 0 the fade-in will start
    bxeq lr
    push {r0-r12, lr}
    bl Cheats_ShouldFixBlackScreen
    cmp r0,#0x1
    pop {r0-r12, lr}
    bx lr

.global hook_ActorDrawContext
hook_ActorDrawContext:
    push {r0-r12, lr}
    bl Actor_rDrawContext
    pop {r0-r12, lr}
    bx lr

.global hook_GameButtonInputs
hook_GameButtonInputs:
.if (_KOR_ || _TWN_)
    push {r0-r12,lr}
    cpy r0,r1
    bl Commands_OverrideGameButtonInputs
    pop {r0-r12,lr}
    str r0,[r4,#0x4]
    bx lr
.else
    push {r0-r12,lr}
    cpy r0,r4
    bl Commands_OverrideGameButtonInputs
    pop {r0-r12,lr}
    add r0,r0,r9,lsl#0x4
    bx lr
.endif

@---------------------
@---------------------

.section .loader
.global hook_into_loader
hook_into_loader:
    push {r0-r12, lr}
    bl loader_main
    pop {r0-r12, lr}
.if (_KOR_ || _TWN_)
    bl 0x100024
.else
    bl 0x100028
.endif
    b  0x100004
