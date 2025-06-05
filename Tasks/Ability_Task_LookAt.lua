local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local Ability_Task_LookAt = UE4.Class(nil, "Ability_Task_LookAt")
function Ability_Task_LookAt:ErrLog(__FUNC_NAME__, ...)
    _SP.LogError("[Ability_Task_LookAt]" .. __FUNC_NAME__, ...)
end

function Ability_Task_LookAt:OnTaskStartBP(Context)
    local ErrLog = function (...) self:ErrLog("::OnTaskStartBP", ...) end

    local OwnerActor = Context:GetOwner()
    if OwnerActor == nil then
        ErrLog("The OwnerActor is nil")
        return
    end

    local OwnerMonster = OwnerActor:Cast(UE4.ASPGameMonsterBase)
    if OwnerMonster == nil then
        ErrLog("The OwnerMonster is nil")
        return
    end

    local LookatCom = OwnerMonster:GetComponentByClass(UE4.USPLookAtComponent:StaticClass())
    if LookatCom == nil then
        ErrLog("LookAt component is nil")
        return
    end

    if self.MultiLookNum > 0 then
        local TargetActors = self:GetActorsForTaskBP(Context)
        LookatCom:MultiSetLookAtParametersWithRPC(self.LookAtType, TargetActors, self.MultiRotateSpeed, self.HoldAngle, self.MaxLookAtDistance, self.ReturnTime, self.Duration, self.MultiLookNum)

        local ScratchPad = self:GetScratchPad(Context)
        if ScratchPad ~= nil then
            ScratchPad.MultiCurrentLookatActor = TargetActors
        end
    else
        -- 目标Index，默认为0即目标数组第一个单位
        local TargetActorIndex = self.TargetActorIndex or 0
        local TargetActor = self:GetSingleActorFromTargetTypeBP(Context, self.TargetActorType, TargetActorIndex)
        LookatCom:SetLookAtParametersWithRPC(self.LookAtType, TargetActor, self.RotateSpeed, self.HoldAngle, self.MaxLookAtDistance, self.ReturnTime, self.Duration)

        local ScratchPad = self:GetScratchPad(Context)
        if ScratchPad ~= nil then
            ScratchPad.CurrentLookatActor = TargetActor
        end
    end
end

function Ability_Task_LookAt:OnTaskTickBP(Context, DeltaTime)
    if self.NeedDynamicLookAt == true then
        local ErrLog = function (...) self:ErrLog("::OnTaskTickBP", ...) end

        local OwnerActor = Context:GetOwner()
        if OwnerActor == nil then
            ErrLog("The OwnerActor is nil")
            return
        end

        local OwnerMonster = OwnerActor:Cast(UE4.ASPGameMonsterBase)
        if OwnerMonster == nil then
            ErrLog("The OwnerMonster is nil")
            return
        end

        local LookatCom = OwnerMonster:GetComponentByClass(UE4.USPLookAtComponent:StaticClass())
        if LookatCom == nil then
            ErrLog("LookAt component is nil")
            return
        end
        if self.MultiLookNum > 0 then
            local TargetActors = self:GetActorsForTaskBP(Context)
            local ScratchPad = self:GetScratchPad(Context)
            if ScratchPad ~= nil then
                local CurLen = TargetActors:Length()
                local CacheLen = ScratchPad.MultiCurrentLookatActor:Length()
                if CurLen ~= CacheLen then
                    LookatCom:MultiSetLookAtParametersWithRPC(self.LookAtType, TargetActors, self.MultiRotateSpeed, self.HoldAngle, self.MaxLookAtDistance, self.ReturnTime, self.Duration, self.MultiLookNum)
                    ScratchPad.MultiCurrentLookatActor = TargetActors
                end
                for i = 1, CurLen do
                    if TargetActors:Get(i) ~= ScratchPad.MultiCurrentLookatActor:Get(i) then
                        LookatCom:MultiSetLookAtParametersWithRPC(self.LookAtType, TargetActors, self.MultiRotateSpeed, self.HoldAngle, self.MaxLookAtDistance, self.ReturnTime, self.Duration, self.MultiLookNum)
                        ScratchPad.MultiCurrentLookatActor = TargetActors
                        return
                    end
                end
            end
            -- LookatCom:MultiSetLookAtParametersWithRPC(self.LookAtType, TargetActors, self.MultiRotateSpeed, self.HoldAngle, self.MaxLookAtDistance, self.ReturnTime, self.Duration)

        else
            local TargetActor = self:GetSingleActorFromTargetTypeBP(Context, self.TargetActorType)
            local ScratchPad = self:GetScratchPad(Context)
            if ScratchPad ~= nil and ScratchPad.CurrentLookatActor ~= TargetActor and TargetActor ~= nil then
                ScratchPad.CurrentLookatActor = TargetActor
                LookatCom:SetLookAtParametersWithRPC(self.LookAtType, TargetActor, self.RotateSpeed, self.HoldAngle, self.MaxLookAtDistance, self.ReturnTime, self.Duration)
                _SP.Log("Ability_Task_LookAt", "Lookat new actor:", TargetActor:GetName())
            end
        end
    end
end

function Ability_Task_LookAt:OnTaskEndBP(Context)
    local ErrLog = function (...) self:ErrLog("::OnTaskEndBP", ...) end

    local OwnerActor = Context:GetOwner()
    if OwnerActor == nil then
        ErrLog("The OwnerActor is nil")
        return
    end

    local OwnerMonster = OwnerActor:Cast(UE4.ASPGameMonsterBase)
    if OwnerMonster == nil then
        ErrLog("ASPGameMonsterBase is nil")
        return
    end

    local lookatCom = OwnerMonster:GetComponentByClass(UE4.USPLookAtComponent:StaticClass())
    if lookatCom == nil then
        ErrLog("LookAt component is nil")
        return
    end

    if self.MultiLookNum > 0 then
        lookatCom:SetLookAtParametersWithRPC(UE4.ELookAtType.Disable, nil, self.MultiRotateSpeed, self.HoldAngle, self.MaxLookAtDistance, self.ReturnTime, self.Duration, 0)
    else
        lookatCom:SetLookAtParametersWithRPC(UE4.ELookAtType.Disable, nil, self.RotateSpeed, self.HoldAngle, self.MaxLookAtDistance, self.ReturnTime, self.Duration)
    end
end

function Ability_Task_LookAt:ResetScratchPadBP(ScratchPad)
    if ScratchPad then
        ScratchPad.MultiCurrentLookatActor = nil
        ScratchPad.CurrentLookatActor = nil
    end
end

function Ability_Task_LookAt:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.LookAt)
end


return Ability_Task_LookAt