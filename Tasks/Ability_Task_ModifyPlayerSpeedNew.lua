local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local Ability_Task_ModifyPlayerSpeedNew = UE4.Class(nil, "Ability_Task_ModifyPlayerSpeedNew")

function Ability_Task_ModifyPlayerSpeedNew:OnTaskStartBP(Context)
    local Character = Context:GetOwner()
    if not _SP.IsValid(Character) then
        --character为空
        _SP.LogWarning("Ability_Task_ModifyPlayerSpeedNew","character is nil")
        return
    else
        if self.UseTaskSpeed then
            --如果未勾选恢复速度，且勾选了使用task速度选项，则修改玩家速度
            _SP.LogWarning("[Test]Ability_Task_ModifyPlayerSpeedNew","SetPlayerSpeed:",self.PlayerCurSpeed)
            Character:SetPlayerSpeed(self.PlayerCurSpeed)
            return
        end
        --如果都未勾选，打印提示log
        _SP.LogWarning("Ability_Task_ModifyPlayerSpeedNew","please chose RecoverPlayerSpeed or UseTaskSpeed")
    end
end

function Ability_Task_ModifyPlayerSpeedNew:OnTaskEndBP(Context)
    local Character = Context:GetOwner()
    --恢复速度
    _SP.LogWarning("[Test]Ability_Task_ModifyPlayerSpeedNew","RecoverPlayerSpeed:")
    Character:RecoverPlayerSpeed()
    return
end

return Ability_Task_ModifyPlayerSpeedNew