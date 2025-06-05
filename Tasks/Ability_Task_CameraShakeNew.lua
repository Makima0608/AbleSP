local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local SPLuaUtility = require("Feature.StarP.Script.System.SPLuaUtility")
local Ability_Task_CameraShakeNew = UE4.Class(nil, "Ability_Task_CameraShakeNew")

local IsValid = UE4.UKismetSystemLibrary.IsValid

function Ability_Task_CameraShakeNew:OnTaskStartBP(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    local TargetArray = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, TargetArray)

    for i = 1, TargetArray:Length() do
        local Target = TargetArray:Get(i)
        local Character
        if Target:IsA(UE4.AMoeGameCharacter) then
            Character = Target
        elseif Target:IsA(UE4.ASPGameSummonBase) then
            -- 召唤物支持，如果技能目标是召唤物，则取其主人为目标
            Character = Target:GetSummonMaster()
        end
        if Character then
            UE4.UMoeCameraManagerSubSystem.Action_Camera_StartSPShakeCustom(Character,tostring(self.ShakeClass),
            self.CameraCustomShakeParam,true)
            if not ScratchPad.Characters then
                ScratchPad.Characters = {}
            end
            table.insert(ScratchPad.Characters, Character)
        else
            Character = Target:Cast(UE4.ASPGameMonsterBase)
            if Character then
                if Character:IsRidden() and Character:GetRiderPawn() then
                    Character = Character:GetRiderPawn()
                    UE4.UMoeCameraManagerSubSystem.Action_Camera_StartSPShakeCustom(Character,tostring(self.ShakeClass),
                    self.CameraCustomShakeParam,true)
                    if not ScratchPad.Characters then
                        ScratchPad.Characters = {}
                    end
                    table.insert(ScratchPad.Characters, Character)
                end
            end
        end
    end
end

function Ability_Task_CameraShakeNew:OnTaskEndBP(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad or not ScratchPad.Characters or #ScratchPad.Characters == 0 then
        return
    end

    local StopMode = self.StopMode
    if StopMode == UE4.EAblePlayCameraShakeStopMode.DontStop then
        return
    end

    for i, Character in ipairs(ScratchPad.Characters) do
        if (IsValid(Character)) then
            UE4.UMoeCameraManagerSubSystem.Action_Camera_StopShake(Character)
        end
    end
end

function Ability_Task_CameraShakeNew:ResetScratchPadBP(ScratchPad)
    if ScratchPad then
        ScratchPad.Characters = {}
    end
end

function Ability_Task_CameraShakeNew:IsSingleFrameBP()
    return false
end

function Ability_Task_CameraShakeNew:GetTaskRealmBP()
    return UE4.EAbleAbilityTaskRealm.ATR_Client
end

function Ability_Task_CameraShakeNew:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.CameraShakeNew)
end

return Ability_Task_CameraShakeNew