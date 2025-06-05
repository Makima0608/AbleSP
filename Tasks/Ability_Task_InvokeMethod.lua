-- 在目标上触发一个函数调用，目前仅支持无参数函数
---@class Ability_Task_InvokeMethod
---@field TaskRealm UE4.EAbleAbilityTaskRealm
---@field FunctionName string
local Ability_Task_InvokeMethod = UE4.Class(nil, "Ability_Task_InvokeMethod")

---@param Context UE4.UAbleAbilityContext
function Ability_Task_InvokeMethod:OnTaskStartBP(Context)
    local Targets = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, Targets)

    for i = 1, Targets:Length() do
        local Target = Targets:Get(i)
        local Func = Target[self.FunctionName]
        if Func then
            if self.bStaticMethod then
                Func()
            else
                Func(Target)
            end
        end
    end
end

return Ability_Task_InvokeMethod