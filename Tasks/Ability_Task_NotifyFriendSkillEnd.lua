---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by hangtian.
--- DateTime: 2024/6/17 下午7:45
---

local function ErrLog(...)
    _SP.LogError("SPAbility", "Ability_Task_NotifyFriendSkillEnd", ...)
end

local function Log(...)
    _SP.Log("SPAbility", "Ability_Task_NotifyFriendSkillEnd", ...)
end

---@class Ability_Task_NotifyFriendSkillEnd
local Ability_Task_NotifyFriendSkillEnd = UE4.Class(nil, "Ability_Task_NotifyFriendSkillEnd")

---@param Context UAblAbilityContext
function Ability_Task_NotifyFriendSkillEnd:OnTaskStartBP(Context)
    Log("OnTaskStartBP")
    if self.m_TaskTargets == nil then
        return
    end
    local len = self.m_TaskTargets:Length()
    for iter = 1, len do
        local TargetType = self.m_TaskTargets:Get(iter)
        local TargetActor = self:GetSingleActorFromTargetTypeBP(Context, TargetType)
        if TargetActor ~= nil and TargetActor:Cast(UE4.ASPGameCharacterBase) then
            ---@type SPGameFriendSkillComponent
            local friendSkillComponent = TargetActor:GetFriendSkillComponent()
            if friendSkillComponent == nil then
                return
            end
            friendSkillComponent:SetWeaponFriendSkillState(true)
        end
    end
end
---@param Context UAblAbilityContext
function Ability_Task_NotifyFriendSkillEnd:OnTaskEndBP(Context)
    Log("OnTaskStartBP")
    if self.m_TaskTargets == nil then
        return
    end
    local len = self.m_TaskTargets:Length()
    for iter = 1, len do
        local TargetType = self.m_TaskTargets:Get(iter)
        local TargetActor = self:GetSingleActorFromTargetTypeBP(Context, TargetType)
        if TargetActor ~= nil and TargetActor:Cast(UE4.ASPGameCharacterBase) then
            ---@type SPGameFriendSkillComponent
            local friendSkillComponent = TargetActor:GetFriendSkillComponent()
            if friendSkillComponent == nil then
                return
            end
            friendSkillComponent:SetWeaponFriendSkillState(false)
        end
    end
end

return Ability_Task_NotifyFriendSkillEnd