local Ability_Task_StopAI = UE4.Class(nil, "Ability_Task_StopAI")

function Ability_Task_StopAI:OnTaskStartBP(Context)
    _SP.Log("SPAbility","Ability_Task_StopAI:OnTaskStartBP")
    local TargetArray = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, TargetArray)
    for i = 1, TargetArray:Length() do
        local Target = TargetArray:Get(i)
        if Target:IsA(UE4.ASPGameMonsterBase) then
            --- 只暂停AI行为而不停止AI
            if self.PauseBehavior then
                Target:PauseAIBehavior()
            else
                Target:StopAI()
            end
        end
    end
end

function Ability_Task_StopAI:OnTaskEndBP(Context, Result)
    _SP.Log("SPAbility","Ability_Task_StopAI:OnTaskEndBP")
    if self.ResumeAIOnEnd then
        local TargetArray = UE4.TArray(UE4.AActor)
        self:GetActorsForTaskBP(Context, TargetArray)
        for i = 1, TargetArray:Length() do
            local Target = TargetArray:Get(i)
            if Target:IsA(UE4.ASPGameMonsterBase) then
                if self.PauseBehavior then
                    Target:ResumeAIBehavior()
                else
                    Target:ResumeAI()
                end
            end
        end
    end
end

return Ability_Task_StopAI