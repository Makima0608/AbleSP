---@class Ability_Task_ForbidAction
local Ability_Task_ForbidAction = UE4.Class(nil, "Ability_Task_ForbidAction")

local S_UE4 = UE4
local S_UnifiedNetworkLib = S_UE4.USPUnifiedNetworkLibrary
local S_IsRelatedToLocalController = S_UnifiedNetworkLib.IsRelatedToLocalController
local IsCausedByAbilityTask = 10

function Ability_Task_ForbidAction:OnTaskStartBP(Context)
    _SP.Log("SPAbilityTask", "Ability_Task_ForbidAction", "OnTaskStartBP")
    if _SP and _SP.IsClient then
        local OwnerActor = Context:GetOwner()
        if OwnerActor == nil then
            _SP.LogWarning("SPAbilityTask", "Ability_Task_ForbidAction", "task owner is nil")
            return
        end
        if not S_IsRelatedToLocalController(OwnerActor, false) then
            _SP.LogWarning("SPAbilityTask", "Ability_Task_ForbidAction", "task owner not local controlled")
            return
        end

        if self.ForbiddenAction == nil then
            return
        end

        _SP.Log("SPAbilityTask", "Ability_Task_ForbidAction", "setting buttons' states")
        for i = 1, self.ForbiddenAction:Length() do
            local buttonName = self.ForbiddenAction:Get(i)
            _SP.Client.SPButtonStateManager:SetDisableByFlag(buttonName, IsCausedByAbilityTask, true)
        end
    end
end

function Ability_Task_ForbidAction:OnTaskTickBP(Context)
end

function Ability_Task_ForbidAction:OnTaskEndBP(Context)
    _SP.Log("SPAbilityTask", "Ability_Task_ForbidAction", "OnTaskEndBP")


    for i = 1, self.ForbiddenAction:Length() do
        local buttonName = self.ForbiddenAction:Get(i)
        _SP.Client.SPButtonStateManager:SetDisableByFlag(buttonName, IsCausedByAbilityTask, false)
    end
end

return Ability_Task_ForbidAction
