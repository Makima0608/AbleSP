
local Ability_Task_PlayAndStopAbility = UE4.Class(nil, "Ability_Task_PlayAndStopAbility")
local UE4_ESPStopAbilityType_Finish = UE4.ESPStopAbilityType.Finish

function Ability_Task_PlayAndStopAbility:IsSingleFrameBP()
    return false
end

function Ability_Task_PlayAndStopAbility:GetTaskRealmBP()
    return UE4.EAbleAbilityTaskRealm.ATR_Server
end

local function IsAbilityExist(abilityId)
    local abilityData = _SP.SPConfigManager:GetConfigById("SPAbilityDataTable", "SPAbility", abilityId)
    if (abilityData) then
        return true
    end

    local passiveAbilityData = _SP.SPConfigManager:GetConfigById("SPPassiveAbilityDataTable", "SPPassiveAbility", abilityId)
    if (passiveAbilityData) then
        return true
    end
    return false
end

---@param Context UAbleAbilityContext
function Ability_Task_PlayAndStopAbility:OnTaskStartBP(Context)
    if self.AbilityId <= 0 then
        return
    end

    if not IsAbilityExist(self.AbilityId) then
        return
    end

    local TargetArray = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, TargetArray)

    for i = 1, TargetArray:Length() do
        local Target = TargetArray:Get(i)
        if Target then
            ---@type USPAbilityComponent
            local AbilityComponent = Target:GetComponentByClass(UE4.USPAbilityComponent:StaticClass())
            if AbilityComponent then
                AbilityComponent:TryActivateAbility(self.AbilityId, Target, Context:GetOwner())
            end
        end
    end
end

function Ability_Task_PlayAndStopAbility:OnTaskEndBP(Context, Result)
    if self.AbilityId <= 0 then
        return
    end

    if not IsAbilityExist(self.AbilityId) then
        return
    end

    local TargetArray = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, TargetArray)

    for i = 1, TargetArray:Length() do
        local Target = TargetArray:Get(i)
        if Target then
            ---@type USPAbilityComponent
            local AbilityComponent = Target:GetComponentByClass(UE4.USPAbilityComponent:StaticClass())
            if AbilityComponent then
                AbilityComponent:TryStopAbility(self.AbilityId, UE4_ESPStopAbilityType_Finish, Context:GetOwner())
            end
        end
    end
end

return Ability_Task_PlayAndStopAbility