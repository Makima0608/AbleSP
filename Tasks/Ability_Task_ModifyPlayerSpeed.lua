local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local Ability_Task_ModifyPlayerSpeed = UE4.Class(nil, "Ability_Task_ModifyPlayerSpeed")

function Ability_Task_ModifyPlayerSpeed:OnTaskStartBP(Context)
    local Character = Context:GetOwner()
    if not _SP.IsValid(Character) then
        --character为空
        _SP.LogWarning("Ability_Task_ModifyPlayerSpeed","character is nil")
        return
    else
        if self.RecoverPlayerSpeed then
            --如果勾選了恢复速度选项，则此task只执行恢复速度
            Character:RecoverPlayerSpeed()
            return
        end
        if self.UseTaskSpeed then
            --如果未勾选恢复速度，且勾选了使用task速度选项，则修改玩家速度
            Character:SetPlayerSpeed(self.PlayerCurSpeed)
            return
        end
        --如果都未勾选，打印提示log
        _SP.LogWarning("Ability_Task_ModifyPlayerSpeed","please chose RecoverPlayerSpeed or UseTaskSpeed")
    end
end

function Ability_Task_ModifyPlayerSpeed:OnTaskEndBP(Context)
end

function Ability_Task_ModifyPlayerSpeed:IsSingleFrameBP()
    return true
end

return Ability_Task_ModifyPlayerSpeed