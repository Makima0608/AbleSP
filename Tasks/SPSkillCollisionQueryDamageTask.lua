local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")

---@class SPSkillCollisionQueryDamageTask : USPSkillCollisionQueryDamageTask
local SPSkillCollisionQueryDamageTask = UE4.Class(nil, "SPSkillCollisionQueryDamageTask")

---OnCalcMaxDamageCount
---计算最大伤害次数
---@param Context UAbleAbilityContext
function SPSkillCollisionQueryDamageTask:OnCalcMaxDamageCount(Context)
    local Owner = Context:GetOwner()
    local AbilityId = Context:GetAbilityId()
    local AbilityUniqueID = Context:GetAbilityUniqueID()

    SPAbilityUtils.DispatchMaxDamageCount(AbilityId, AbilityUniqueID, Owner, 0, self.m_Interval, self:GetDuration())
end

return SPSkillCollisionQueryDamageTask
