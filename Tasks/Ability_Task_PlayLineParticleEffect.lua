local SPAbilityGameplayTags = require("Feature.StarP.Script.System.Ability.SPAbilityGameplayTagsConfig")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")

local Ability_Task_PlayLineParticleEffect = UE4.Class(nil, "Ability_Task_PlayLineParticleEffect")

---@param Context UAblAbilityContext
function Ability_Task_PlayLineParticleEffect:OnTaskStartBP(Context)
    if not _SPFeature.GEnv.IsDS then
        local effectTemplate = self.EffectTemplate

        self.Target = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_TargetActor)
        self.Owner = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_Self)

        if not _SP.SPAbilityUtils.IsValid(self.Target) or not _SP.SPAbilityUtils.IsValid(self.Owner) then
            return
        end

        local targetLocation, sourceLocation = self:GetTargetAndSourceLocation(self.Target, self.Owner)

        if effectTemplate then
            self.SpawnedEffect =
            UE4.UGameplayStatics.SpawnEmitterAtLocation(self.Owner:GetWorld(), effectTemplate, sourceLocation, true)
        end

        if self.SpawnedEffect then
            self:SetupAndRefreshLine(targetLocation, sourceLocation)
        end
    end

    if _SPFeature.GEnv.IsDS or _SP.IsStandalone then
        local abilityEventPayload = SPAbilityUtils.CreateAbilityEvent()
        abilityEventPayload.EventTag = SPAbilityGameplayTags.StarP_Ability_Event_MonsterLinkBegin
        abilityEventPayload.Instigator = Context:GetOwner()
        SPAbilityUtils.BroadcastAbilityEvent(Context:GetOwner(), abilityEventPayload)
    end
end

---@param Context UAblAbilityContext
function Ability_Task_PlayLineParticleEffect:OnTaskEndBP(Context)
    if not _SPFeature.GEnv.IsDS then
        self:RemoveLine()
    end

    if _SPFeature.GEnv.IsDS or _SP.IsStandalone then
        local abilityEventPayload = SPAbilityUtils.CreateAbilityEvent()
        abilityEventPayload.EventTag = SPAbilityGameplayTags.StarP_Ability_Event_MonsterLinkEnd
        abilityEventPayload.Instigator = Context:GetOwner()
        SPAbilityUtils.BroadcastAbilityEvent(Context:GetOwner(), abilityEventPayload)
    end
end

function Ability_Task_PlayLineParticleEffect:OnTaskTickBP(Context, DeltaTime)
    if not _SPFeature.GEnv.IsDS then
        if not _SP.SPAbilityUtils.IsValid(self.Target) or not _SP.SPAbilityUtils.IsValid(self.Owner) then
            self:RemoveLine()
            return
        end
        local targetLocation, sourceLocation = self:GetTargetAndSourceLocation(self.Target, self.Owner)
        self:SetupAndRefreshLine(targetLocation, sourceLocation)
    end
end

function Ability_Task_PlayLineParticleEffect:GetTargetAndSourceLocation(TargetActor, SourceActor)
    local targetLocation = TargetActor:K2_GetActorLocation()
    local sourceLocation = SourceActor:K2_GetActorLocation()

    if self.TargetUseSocket then
        local targetMeshComponent = TargetActor:GetComponentByClass(UE4.USkeletalMeshComponent.StaticClass())
        if targetMeshComponent then
            targetLocation = targetMeshComponent:GetSocketLocation(self.TargetSocketName)
        end
    end

    if self.SourceUseSocket then
        local sourceMeshComponent = SourceActor:GetComponentByClass(UE4.USkeletalMeshComponent.StaticClass())
        if sourceMeshComponent then
            sourceLocation = sourceMeshComponent:GetSocketLocation(self.SourceSocketName)
        end
    end

    return targetLocation, sourceLocation
end

function Ability_Task_PlayLineParticleEffect:SetupAndRefreshLine(TargetLocation, SourceLocation)
    if self.SpawnedEffect == nil then
        return
    end

    if not UE4.UKismetSystemLibrary.IsValid(self.SpawnedEffect) then
        return
    end

    local delta = TargetLocation - SourceLocation
    local scale = 1
    if self.scale ~= nil then
        scale = self.scale
    end
    local distortion = UE4.FVector(0, 0, -10 * scale)
    self.SpawnedEffect:K2_SetWorldLocation(SourceLocation)
    self.SpawnedEffect:SetVectorParameter("distortion", distortion)
    self.SpawnedEffect:SetVectorParameter("TargetLocation", delta)
end

function Ability_Task_PlayLineParticleEffect:RemoveLine()
    if self.SpawnedEffect then
        UE4.USPGameLibrary.DeactivateParticleSystem(self.SpawnedEffect)
        self.SpawnedEffect = nil
    end
end

return Ability_Task_PlayLineParticleEffect
