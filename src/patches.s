.arm

.section .patch_Gfx_Update
.global Gfx_Update_patch
Gfx_Update_patch:
    b hook_into_Gfx_Update

.section .patch_before_GlobalContext_Update
.global before_GlobalContext_Update_patch
before_GlobalContext_Update_patch:
    bl hook_before_GlobalContext_Update

.section .patch_after_GlobalContext_Update
.global after_GlobalContext_Update_patch
after_GlobalContext_Update_patch:
    b hook_after_GlobalContext_Update

.section .patch_loader
.global loader_patch
loader_patch:
    b hook_into_loader

.section .patch_PlaySound
.global PlaySound_patch
PlaySound_patch:
    b hook_PlaySound

.section .patch_SetBGMEntrance
.global SetBGMEntrance_patch
SetBGMEntrance_patch:
    b hook_SetBGMEntrance

.section .patch_SetBGMDayNight
.global SetBGMDayNight_patch
SetBGMDayNight_patch:
    b hook_SetBGMDayNight

.section .patch_SetBGMEvent
.global SetBGMEvent_patch
SetBGMEvent_patch:
    b hook_SetBGMEvent

.section .patch_TurboTextAdvance
.global TurboTextAdvance_patch
TurboTextAdvance_patch:
    bl hook_TurboTextAdvance

.section .patch_TurboTextClose
.global TurboTextClose_patch
TurboTextClose_patch:
    bl hook_TurboTextClose

.section .patch_TurboTextSignalNPC
.global TurboTextSignalNPC_patch
TurboTextSignalNPC_patch:
    bl hook_TurboTextSignalNPC

.section .patch_InstantTextFirstLine
.global InstantTextFirstLine_patch
InstantTextFirstLine_patch:
    bl hook_InstantTextFirstLine

.section .patch_InstantTextBoxBreak
.global InstantTextBoxBreak_patch
InstantTextBoxBreak_patch:
    b hook_InstantTextBoxBreak

.section .patch_InstantTextRemoveOff
.global InstantTextRemoveOff_patch
InstantTextRemoveOff_patch:
    b hook_InstantTextRemoveOff

.section .patch_ItemUsability
.global ItemUsability_patch
ItemUsability_patch:
    bl hook_ItemUsability

.section .patch_ItemUsability_Shield
.global ItemUsabilityShield_patch
ItemUsabilityShield_patch:
    b hook_ItemUsability_Shield

.section .patch_SleepQueryCallback
.global SleepQueryCallback_patch
SleepQueryCallback_patch:
    b hook_Gfx_SleepQueryCallback

.section .patch_OverrideSceneSetup
.global OverrideSceneSetup_patch
OverrideSceneSetup_patch:
    bl hook_OverrideSceneSetup

.section .patch_LoadGame
.global LoadGame_patch
LoadGame_patch:
    bl hook_LoadGame

.section .patch_CameraUpdate
.global CameraUpdate_patch
CameraUpdate_patch:
    bl hook_CameraUpdate

.section .patch_Actor_UpdateAll
    bl hook_Actor_UpdateAll

.section .patch_before_GameState_Loop
    bl hook_before_GameState_Loop

.section .patch_after_GameState_Update
    bl hook_after_GameState_Update

.section .patch_ActorDrawContext
    bl Actor_rDrawContext

.section .patch_BlackScreenFix
    bl hook_BlackScreenFix

.section .patch_GameButtonInputs
    bl hook_GameButtonInputs
