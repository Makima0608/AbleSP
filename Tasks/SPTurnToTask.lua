---@class SPTurnToTask : UAbleAbilityTask
local SPTurnToTask = UE4.Class(nil, "SPTurnToTask")

function SPTurnToTask:GetCustomAngularVelocity(TargetActor)
    if self.m_bScaleAngularVelocity then
        if _SP.IsValid(TargetActor) and TargetActor.MonsterConfig then
            return (TargetActor.MonsterConfig.turnToScale and TargetActor.MonsterConfig.turnToScale or 1) * self.m_AngularVelocity
        end
    end
    return 1.0 * self.m_AngularVelocity
end

return SPTurnToTask
