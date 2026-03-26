.arm
.text

.macro HOOK name
    .global hook_\name
hook_\name:
.endm

.section .loader
HOOK into_loader
    push {lr}
    bl loader_main
    bl nninitRegion
    pop {lr}
    bx lr

@ Place hooks in this section if they're valid for all versions
@ (USA, EUR, JPN, KOR, TWN, DEMO_USA, DEMO_EUR)
.section .asm_hooks.all_versions

HOOK into_Gfx_Update
    push {r0-r12,lr}
    bl advance_main
    pop {r0-r12,lr}
    pop {r4-r8, pc}

HOOK before_Play_Update
    push {r0-r12,lr}
    bl before_Play_Update
    pop {r0-r12,lr}
.if _KOR_TWN_
    cpy r8,r0
.else
    cpy r7,r0
.endif
    bx lr

HOOK after_Play_Update
    push {lr}
    push {r0-r12}
    bl after_Play_Update
    pop {r0-r12}
    bl Play_Draw
    push {r0-r12}
    bl after_Play_Draw
    pop {r0-r12}
    pop {pc}

HOOK HaltActors
    push {r0-r12,lr}
    bl Scene_HaltActorsEnabled
    cmp r0,#0x0
    pop {r0-r12,lr}
    bne PlayUpdate_PostActorUpdate
    cmp r0,#0x0
    bx lr

HOOK before_GameState_Loop
.if _KOR_TWN_
    REG_GAMESTATE .req r4
    REG_ORIG_PARAM .req r9
.elseif _DEMO_
    REG_GAMESTATE .req r4
    REG_ORIG_PARAM .req r6
.else
    REG_GAMESTATE .req r5
    REG_ORIG_PARAM .req r4
.endif
    push {r0-r12,lr}
    cpy r0, REG_GAMESTATE
    bl before_GameState_Loop
    pop {r0-r12,lr}
    cpy r0, REG_ORIG_PARAM
    bx lr
.unreq REG_GAMESTATE
.unreq REG_ORIG_PARAM

HOOK WrapGameStateDraw
    push {r0-r12,lr}
    bl checkFastForward
    cmp r0,#0x0
    pop {r0-r12,lr}
    beq GameState_Draw @ handles drawing screen
    bx lr

HOOK ActorDrawContext
    push {r0-r12,lr}
    bl Actor_rDrawContext
    pop {r0-r12,lr}
    bx lr

HOOK GameButtonInputs
.if _KOR_TWN_
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

HOOK Audio_PlayFanfare
    push {r1-r12,lr}
    bl Cheats_RemoveBGM
    pop {r1-r12,lr}
    str r0,[r5,#0x88]
    bx lr

HOOK SetBGMEntrance
    push {r1-r12,lr}
    bl Cheats_RemoveBGM
    pop {r1-r12,lr}
    cpy r4,r0
    bx lr

HOOK SetBGMDayNight
.if _KOR_TWN_
    cpy r4,r0
.else
    cpy r5,r0
.endif
    push {r1-r12,lr}
    bl Cheats_RemoveBGM
    pop {r1-r12,lr}
    bx lr

HOOK SetBGMEvent
    push {r0,r2-r12,lr}
    cpy r0,r1
    bl Cheats_RemoveBGM
    cpy r1,r0
    pop {r0,r2-r12,lr}
    cpy r6,r0
    bx lr

HOOK ParseTextCharacter
.if _KOR_TWN_
    REG_ORIG_PARAM .req r9
    REG_TEXT_OBJECT .req r11 @ overwritten after this hook
    ldr r11,[sp,#0x48]
    REG_CHAR_INDEX .req r6
.else
    REG_ORIG_PARAM .req r6
    REG_TEXT_OBJECT .req r5
    REG_CHAR_INDEX .req r9
.endif
    push {r0-r12,lr}
    cpy r0,REG_TEXT_OBJECT
    @ r1: nextChars
    cpy r2,REG_CHAR_INDEX @ loop counter
    bl Message_ApplyInstantText
    pop {r0-r12,lr}
    cpy r0,REG_ORIG_PARAM
    bx lr
.unreq REG_ORIG_PARAM
.unreq REG_TEXT_OBJECT
.unreq REG_CHAR_INDEX

HOOK ParseTextControlCode
.if _KOR_TWN_
    REG_CC_ID .req r1
    REG_ORIG_PARAM .req r9
.else
    REG_CC_ID .req r0
    REG_ORIG_PARAM .req r6
.endif
    ldrb REG_CC_ID,[REG_ORIG_PARAM,#0x4] @ Control Code identifier
    push {r0-r12,lr}
    cpy r0,REG_CC_ID
    bl Message_ShouldSkipTextControlCode
    cmp r0,#0x0
    pop {r0-r12,lr}
    addne lr,lr,#0x8 @ Skip control code
    bx lr
.unreq REG_CC_ID
.unreq REG_ORIG_PARAM

HOOK TurboTextAdvance
HOOK TurboTextClose
.if _KOR_TWN_
    bic r0,r0,r1
.endif
    push {r0-r12,lr}
    bl Message_ShouldAdvanceTurboText
    cmp r0,#0x0
    pop {r0-r12,lr}
    addne lr,lr,#0x4 @ Skip branch, proceed with text advance
    cmp r0,#0x0
    bx lr

HOOK TurboTextSignalNPC
    @ This fixes NPCs like Anju getting stuck in
    @ talking action after giving an item.
    movne r4,#0x1
    push {r0-r12,lr}
    bl Message_ShouldAdvanceTurboText
    cmp r0,#0x0
    pop {r0-r12,lr}
    movne r4,#0x1
    bx lr

HOOK ItemUsability_AnyArea
.if !_KOR_TWN_
    push {r1-r10,r12,lr}
    cpy r0,r11 @ InterfaceContext
    bl Cheats_OverrideItemRestrictions
    cpy r11,r0
    pop {r1-r10,r12,lr}
    ldrb r0,[r11,#0x29F]
    bx lr
.else
    push {r1-r12,lr}
    bl Cheats_OverrideItemRestrictions
    pop {r1-r12,lr}
    str r0,[sp,#0x8]
    bx lr
.endif

HOOK ItemUsability_AnyAction
    push {r0-r12,lr}
    bl Cheats_ForceUsableItems
    pop {r0-r12,lr}
    cmp r0,#0x17
    bx lr

HOOK SleepQueryCallback
    push {r0-r12,lr}
    bl Gfx_SleepQueryCallback
    pop {r0-r12,lr}
    ldmia sp!,{r4,pc}

HOOK OverrideSceneLayer
    push {r0-r12,lr}
    bl Warps_OverrideSceneLayer
    pop {r0-r12,lr}
    mov r0,#0x2
    bx lr

HOOK LoadGame
    add r0,r5,r4
    push {r0-r12,lr}
    bl Settings_LoadGame
    pop {r0-r12,lr}
    bx lr

HOOK CameraUpdate
.if _KOR_TWN_
    cpy r4,r1
.else
    cpy r6,r0
.endif
    push {r0-r12,lr}
    bl Camera_OnCameraUpdate
    cmp r0,#0x0
    pop {r0-r12,lr}
    bxeq lr
.if _KOR_TWN_
    ldmia sp!,{r0,r1,r4-r11,pc}
.else
    ldmia sp!,{r4-r11,pc}
.endif

HOOK BlackScreenFix
    cmp r0,#0x0 @ cutscene pointer, if 0 the fade-in will start
    bxeq lr
    push {r0-r12,lr}
    bl Cheats_ShouldFixBlackScreen
    cmp r0,#0x1
    pop {r0-r12,lr}
    bx lr
