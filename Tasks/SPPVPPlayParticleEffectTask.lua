---@class SPPVPPlayParticleEffectTask : UAbleAbilityTask
local SPPVPPlayParticleEffectTask = UE4.Class(nil, "SPPVPPlayParticleEffectTask")

function SPPVPPlayParticleEffectTask:SetParticleEffectPos_Lua(Context)
    if not self.IsSelectCapsuleTopAsEffectPoint then
        return
    end

    local owner = Context:GetOwner()
    if owner:GetSPActorType() == UE4.ESPActorType.Pet then
        local capsuleComp = owner:GetComponentByClass(UE4.UCapsuleComponent.StaticClass())
        if not capsuleComp or not capsuleComp:IsValid() then
            _SP.Log("SPAbility", "SPPVPPlayParticleEffectTask", "Not Found CapsuleComponent")
            return
        end

        local height = capsuleComp:GetScaledCapsuleHalfHeight() or 0
        local LocationOffset = UE4.FVector(0, 0, height)
        _SP.Log("SPAbility", "SPPVPPlayParticleEffectTask", "LocationOffset:", LocationOffset)
        self.m_Location.m_Offset = LocationOffset
    end
end

return SPPVPPlayParticleEffectTask