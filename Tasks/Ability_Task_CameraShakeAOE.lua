local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local SPLuaUtility = require("Feature.StarP.Script.System.SPLuaUtility")
local Ability_Task_CameraShakeAOE = UE4.Class(nil, "Ability_Task_CameraShakeAOE")

local IsValid = UE4.UKismetSystemLibrary.IsValid

function Ability_Task_CameraShakeAOE:OnTaskStartBP(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    local playerController = UE4.UGameplayStatics.GetPlayerController(_SP.GetCurrentWorld(), 0)
    local playerPawn = UE4.USPGameLibrary.FindRelatedPlayerPawnByController(playerController)
    if not _SP.IsValid(playerPawn) then
        return
    end
    -- 如果归一化曲线为空，则认为不需要距离衰减
    if not _SP.IsValid(self.CameraShakeAttenuationByDist) then
        UE4.UMoeCameraManagerSubSystem.Action_Camera_StartSPShakeCustom(playerPawn,tostring(self.ShakeClass),
        self.CameraCustomShakeParam,true)
        table.insert(ScratchPad.Characters, playerPawn)
        return
    end

    local playerLocation = playerPawn:K2_GetActorLocation()
    local owner = Context:GetOwner()
    local ownerLocation = owner:K2_GetActorLocation()
    local distance = UE4.UKismetMathLibrary.Vector_Distance2D(ownerLocation, playerLocation)
    local mapDistance = UE4.UKismetMathLibrary.MapRangeClamped(distance,0,self.MaxDist,0,1)
    local attenuationByDist = self.CameraShakeAttenuationByDist:GetFloatValue(mapDistance)
    UE4.UMoeCameraManagerSubSystem.Action_Camera_StartSPShakeCustom(playerPawn,tostring(self.ShakeClass),
    self.CameraCustomShakeParam,true,attenuationByDist)
    table.insert(ScratchPad.Characters, playerPawn)
end

function Ability_Task_CameraShakeAOE:OnTaskEndBP(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad or #ScratchPad.Characters == 0 then
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

function Ability_Task_CameraShakeAOE:ResetScratchPadBP(ScratchPad)
    if ScratchPad then
        ScratchPad.Characters = {}
    end
end

function Ability_Task_CameraShakeAOE:IsSingleFrameBP()
    return false
end

function Ability_Task_CameraShakeAOE:GetTaskRealmBP()
    return UE4.EAbleAbilityTaskRealm.ATR_Client
end

function Ability_Task_CameraShakeAOE:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.CameraShakeAOE)
end

return Ability_Task_CameraShakeAOE