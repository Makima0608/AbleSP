
local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local SPGameSummonManager = require("Feature.StarP.Script.System.SPGameSummonManager")
local Ability_Task_SummonAction = UE4.Class(nil, "Ability_Task_SummonAction")

--- 召唤行为类型
local SummonActionType = {
    ActivateAI = 0,   --- 激活全部召唤物AI
    DestroySummonBySummonID = 1,  --- 销毁指定Owner里对应SummonID的召唤物
    DestroySummonByOwner = 2,  --- 销毁指定Owner的全部召唤物
    DestroyAllSummon = 3,   --- 销毁全部召唤物
}

---@BP Member Variable ImmediateDestroy true 立即销毁  false 走召唤物正常死亡逻辑

function Ability_Task_SummonAction:GetTaskRealmBP()
    return UE4.EAbleAbilityTaskRealm.ATR_Server
end

function Ability_Task_SummonAction:OnTaskStartBP(Context)
    _SP.Log("SPAbilityTask", "Ability_Task_SummonAction:OnTaskStartBP")

    local TargetArray = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, TargetArray)
    --- 激活召唤物行为
    for i = 1, TargetArray:Length() do
        local TargetActor = TargetArray:Get(i)
        self:ActivateSummonAction(TargetActor)
    end

    if self.SummonAction == SummonActionType.DestroyAllSummon then
        self:ActivateDestroyAllSummonAction()
    end
end

function Ability_Task_SummonAction:OnTaskEndBP(Context)
    _SP.Log("SPAbilityTask", "Ability_Task_SummonAction:OnTaskEndBP")
    if not self.StopOnEnd then
        return
    end

    local TargetArray = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, TargetArray)
    --- 关闭召唤物行为
    for i = 1, TargetArray:Length() do
        local TargetActor = TargetArray:Get(i)
        self:DeactivateSummonAction(TargetActor)
    end
end

function Ability_Task_SummonAction:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.TemplatePad)
end

function Ability_Task_SummonAction:ActivateSummonAction(Target)
    _SP.Log("SPAbilityTask", "Ability_Task_SummonAction:ActivateTargetSummonAction")

    if not _SP.IsValid(Target) then
        return
    end

    local TargetSummons = SPGameSummonManager:GetSummonsByOwner(Target)
    if TargetSummons and next(TargetSummons) then
        for _, summon in pairs(TargetSummons) do
            if self.SummonAction == SummonActionType.ActivateAI then
                self:ActivateAI(summon, true)
            end
        end
    end

    if self.SummonAction == SummonActionType.DestroySummonByOwner then
        self:ActivateDestroySummonByOwner(Target)
    elseif self.SummonAction == SummonActionType.DestroySummonBySummonID then
        _SP.Log("SPAbilityTask", "Ability_Task_SummonAction:SummonID", self.SummonID)
        self:ActivateDestroySummonByOwner(Target, self.SummonID)
    end
end

function Ability_Task_SummonAction:DeactivateSummonAction(Target)
    _SP.Log("SPAbilityTask", "Ability_Task_SummonAction:DeactivateSummonAction")
    if not _SP.IsValid(Target) then
        return
    end

    local TargetSummons = SPGameSummonManager:GetSummonsByOwner(Target)
    if TargetSummons and next(TargetSummons) then
        for _, summon in pairs(TargetSummons) do
            if self.SummonAction == SummonActionType.ActivateAI then
                self:ActivateAI(summon, false)
            end
        end
    end
end

---激活全部召唤物AI
---@param Summon SPGameSummonBase
---@param bActivate boolean
function Ability_Task_SummonAction:ActivateAI(Summon, bActivate)
    if _SP.IsValid(Summon) and Summon.ActivateAI then
        _SP.Log("SPAbilityTask", "Ability_Task_SummonAction:ActivateAI", Summon:GetName(), bActivate)
        Summon:ActivateAI(bActivate)
    end
end

function Ability_Task_SummonAction:ActivateDestroySummonBySummonID(SummonID)
    SPGameSummonManager:KillOneSummon(SummonID, self.ImmediateDestroy)
end

function Ability_Task_SummonAction:ActivateDestroySummonByOwner(SummonOwner, SummonID)
    --self.ImmediateDestroy true 立即销毁  false 走召唤物正常死亡逻辑
    SPGameSummonManager:KillSummonsByOwner(SummonOwner, SummonID, self.ImmediateDestroy)
end

function Ability_Task_SummonAction:ActivateDestroyAllSummonAction()
    SPGameSummonManager:KillAllSummon(self.ImmediateDestroy)
end


return Ability_Task_SummonAction