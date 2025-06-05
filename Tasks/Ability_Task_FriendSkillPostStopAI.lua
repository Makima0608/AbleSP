local Ability_Task_FriendSkillPostStopAI = UE4.Class(nil, "Ability_Task_FriendSkillPostStopAI")

function Ability_Task_FriendSkillPostStopAI:OnTaskStartBP(Context)
    local TargetArray = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, TargetArray)
    for i = 1, TargetArray:Length() do
        local Target = TargetArray:Get(i)
        if Target:IsA(UE4.ASPGameCharacterBase) then
            local friendSkillComp = Target:GetFriendSkillComponent()
            if not friendSkillComp then
                return
            end
            friendSkillComp:PostStopAI()
        end
    end
end

return Ability_Task_FriendSkillPostStopAI