---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by hangtian.
--- DateTime: 2024/6/17 下午7:45
---
local StateEnum = {
    ActionState = 0,
    MotionState = 1,
    StageState = 2,
}

local function ErrLog(...)
    _SP.LogWarning("SPAbility", "Ability_Task_ChangeCharState", ...)
end

local function Log(...)
    _SP.Log("SPAbility", "Ability_Task_ChangeCharState", ...)
end

---@class Ability_Task_ChangeCharState
---@field ChangeActionState number
local Ability_Task_ChangeCharState = UE4.Class(nil, "Ability_Task_ChangeCharState")
---@param Context UAblAbilityContext
function Ability_Task_ChangeCharState:OnTaskStartBP(Context)
    Log("OnTaskStartBP")
    if self.m_TaskTargets == nil then
        return
    end
    if (not self.bChangeActionState) and (not self.bChangeMotionState) and (not self.bChangeStageState) then
        Log("must modify a State: Action State or Motion State")
        return
    end
    if self.bChangeActionState then
        self:ChangeState(Context,StateEnum.ActionState)
    end
    if self.bChangeMotionState then
        self:ChangeState(Context,StateEnum.MotionState)
    end
    if self.bChangeStageState then
        self:ChangeState(Context,StateEnum.StageState)
    end
end

function Ability_Task_ChangeCharState:ChangeState(Context,State)
    local len = self.m_TaskTargets:Length()
    for iter = 1, len do
        local TargetType = self.m_TaskTargets:Get(iter)
        local TargetActor = self:GetSingleActorFromTargetTypeBP(Context, TargetType)
        if TargetActor and TargetActor:IsA(UE4.ASPGameCharacterBase) then
            if State == StateEnum.ActionState then
                if self:GetTaskRealmBP() == UE4.EAbleAbilityTaskRealm.ATR_Client then
                    local enterSuccess = TargetActor:TryEnterActionState_New(self.TargetActionState, nil, UE4.EStateIdOffsetType.SP, false)
                    if not enterSuccess then
                        ErrLog("TryEnterActionState failed " .. self.TargetActionState)
                    end
                else
                    local enterSuccess = TargetActor:TryEnterActionState_New(self.TargetActionState, nil, UE4.EStateIdOffsetType.SP, true)
                    if not enterSuccess then
                        ErrLog("TryEnterActionState failed " .. self.TargetActionState)
                    end
                end
            elseif State == StateEnum.MotionState then
                if self:GetTaskRealmBP() == UE4.EAbleAbilityTaskRealm.ATR_Client then
                    local enterSuccess = TargetActor:TryEnterMotionState_New(self.TargetMotionState, nil, UE4.EStateIdOffsetType.SP, false)
                    if not enterSuccess then
                        ErrLog("TryEnterMotionState failed " .. self.TargetMotionState)
                    end
                else
                    local enterSuccess = TargetActor:TryEnterMotionState_New(self.TargetMotionState, nil, UE4.EStateIdOffsetType.SP, true)
                    if not enterSuccess then
                        ErrLog("TryEnterMotionState failed " .. self.TargetMotionState)
                    end
                end
            elseif State == StateEnum.StageState then
                local enterSuccess = TargetActor:TryEnterStageState_New(self.TargetStageState, nil, UE4.EStateIdOffsetType.SP, true)
                if not enterSuccess then
                    ErrLog("TryEnterStageState failed " .. self.TargetStageState)
                end
            end
        elseif TargetActor and TargetActor:IsA(UE4.ASPGameMonsterBase) then
            if State == StateEnum.ActionState then
                local enterSuccess = TargetActor:TryEnterActionState_New(self.TargetActionState, nil, UE4.EStateIdOffsetType.SP, true)
                if not enterSuccess then
                    ErrLog("TryEnterActionState failed " .. self.TargetActionState)
                end
            elseif State == StateEnum.MotionState then
                local enterSuccess = TargetActor:TryEnterMotionState_New(self.TargetMotionState, nil, UE4.EStateIdOffsetType.SP, true)
                if not enterSuccess then
                    ErrLog("TryEnterMotionState failed " .. self.TargetMotionState)
                end
            elseif State == StateEnum.StageState then
                local enterSuccess = TargetActor:TryEnterStageState_New(self.TargetStageState, nil, UE4.EStateIdOffsetType.SP, true)
                if not enterSuccess then
                    ErrLog("TryEnterStageState failed " .. self.TargetStageState)
                end
            end
        end
    end
end

function Ability_Task_ChangeCharState:OnTaskEndBP(Context)
    Log("OnTaskEndBP")
    if self.IsDuration == true then
        if self.m_TaskTargets == nil then
            return
        end
        if (not self.bChangeActionState) and (not self.bChangeMotionState) then
            Log("must leave a State: Action State or Motion State")
            return
        end
        local len = self.m_TaskTargets:Length()
        for iter = 1, len do
            local TargetType = self.m_TaskTargets:Get(iter)
            local TargetActor = self:GetSingleActorFromTargetTypeBP(Context, TargetType)
            if TargetActor and (TargetActor:IsA(UE4.ASPGameCharacterBase) or TargetActor:IsA(UE4.ASPGameMonsterBase)) then
                if self.bChangeActionState and TargetActor.TryLeaveActionState_New then
                    if TargetActor:IsA(UE4.ASPGameCharacterBase) and self:GetTaskRealmBP() == UE4.EAbleAbilityTaskRealm.ATR_Client then
                        TargetActor:TryLeaveActionState_New(self.TargetActionState, UE4.EStateIdOffsetType.SP, false)
                    else
                        TargetActor:TryLeaveActionState_New(self.TargetActionState, UE4.EStateIdOffsetType.SP, true)
                    end
                end
                if self.bChangeMotionState and TargetActor.TryLeaveMotionState_New then
                    if TargetActor:IsA(UE4.ASPGameCharacterBase) and self:GetTaskRealmBP() == UE4.EAbleAbilityTaskRealm.ATR_Client then
                        TargetActor:TryLeaveMotionState_New(self.TargetMotionState, UE4.EStateIdOffsetType.SP, false)
                    else
                        TargetActor:TryLeaveMotionState_New(self.TargetMotionState, UE4.EStateIdOffsetType.SP, true)
                    end
                end
                if self.bChangeStageState and TargetActor.TryLeaveStageState_New then
                    TargetActor:TryLeaveStageState_New(self.TargetStageState, UE4.EStateIdOffsetType.SP, true)
                end
            end
        end
    end
end

return Ability_Task_ChangeCharState