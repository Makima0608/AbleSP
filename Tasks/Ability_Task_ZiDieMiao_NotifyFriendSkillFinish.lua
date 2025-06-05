local Ability_Task_ZiDieMiao_NotifyFriendSkillFinish = UE4.Class(nil, "Ability_Task_ZiDieMiao_NotifyFriendSkillFinish")
local FriendSkillStateType = require("Feature.StarP.Script.System.Character.Define.FriendSkillStateType")

---@param Context UAblAbilityContext
function Ability_Task_ZiDieMiao_NotifyFriendSkillFinish:OnTaskStartBP(Context)
    local TargetArray = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, TargetArray)
    for i = 1, TargetArray:Length() do
        local Target = TargetArray:Get(i)
        if Target.GetFriendSkillComponent == nil then
            break
        end
        local FriendSkillCom = Target:GetFriendSkillComponent()
        if not _SP.IsValid(FriendSkillCom) then
            break
        end
        if self.IsMount then
            FriendSkillCom:SetFriendState(FriendSkillStateType.State.FriendSkillFinish)
        else
            FriendSkillCom:SetFriendState(FriendSkillStateType.State.FriendSkillStateExit)
        end
    end
end

return Ability_Task_ZiDieMiao_NotifyFriendSkillFinish