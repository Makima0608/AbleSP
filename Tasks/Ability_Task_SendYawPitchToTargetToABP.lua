
local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local Ability_Task_SendYawPitchToTargetToABP = UE4.Class(nil, "Ability_Task_SendYawPitchToTargetToABP")

---@param Context UAbleAbilityContext
function Ability_Task_SendYawPitchToTargetToABP:OnTaskStartBP(Context)
    if not self:IsLocalCharacter(Context) then
        return
    end

	self.AnimStateComp = nil
	local Player = Context:GetOwner()
	if _SP.IsValid(Player) then
		self.AnimStateComp = Player:GetComponentByClass(UE4.USPAnimStateComponent.StaticClass())
		self:CheckTargetToSetPitchAndYaw(Context, Player)
	end
end

function Ability_Task_SendYawPitchToTargetToABP:OnTaskTickBP(Context, DeltaTime)
    if not self:IsLocalCharacter(Context) then
        return
    end

    local Player = Context:GetOwner()
    if _SP.IsValid(Player) then
        self:CheckTargetToSetPitchAndYaw(Context, Player)
    end
end

---@param Context UAbleAbilityContext
---@param Result EAbleAbilityTaskResult
function Ability_Task_SendYawPitchToTargetToABP:OnTaskEndBP(Context, Result)
    if not self:IsLocalCharacter(Context) then
        return
    end
	if self.AnimStateComp then
		self.AnimStateComp:SetSPAbilityYaw(0.0)
        self.AnimStateComp:SetSPAbilityPitch(0.0)
        self.AnimStateComp:SetEnableAbilityAO(false)
	end
end

function Ability_Task_SendYawPitchToTargetToABP:CheckTargetToSetPitchAndYaw(Context, Player)
	if self.AnimStateComp then
        local TargetActor = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_TargetActor)
        if TargetActor then
            local TargetPosition = TargetActor:K2_GetActorLocation()
            local PlayerPosition = Player:K2_GetActorLocation();
            local IgnoreActors = UE4.TArray(UE4.AActor)
            IgnoreActors:Add(Player)
            if self.TargetPosType == UE4.ESPAbilityTaskTargetPosType.WeakPoint then
                if not UE4.USPGameLibrary.TryGetExposedWeakPointPosition(TargetPosition, TargetActor, PlayerPosition, IgnoreActors) then
                    TargetPosition = TargetActor:K2_GetActorLocation()
                end
            elseif self.TargetPosType == UE4.ESPAbilityTaskTargetPosType.Bone then
                if TargetActor.TryGetBoneHitLocation then
                    TargetPosition = TargetActor:TryGetBoneHitLocation(self.TargetBoneName)
                end
            end
            local CameraManager = UE4.UGameplayStatics.GetPlayerCameraManager(_SP.GetCurrentWorld(), 0)
            local CameraRot = CameraManager:GetCameraRotation()
            local ToTargetRot = UE4.UKismetMathLibrary.FindLookAtRotation(PlayerPosition, TargetPosition)
            --UE4.UKismetSystemLibrary.DrawDebugLine(_SP.GetCurrentWorld(), PlayerPosition, TargetPosition, UE4.FLinearColor(0,0,1,1), 5)
            local DeltaRot = ToTargetRot - CameraRot
            self.AnimStateComp:SetSPAbilityYaw(DeltaRot.Yaw);
            self.AnimStateComp:SetSPAbilityPitch(DeltaRot.Pitch);
            self.AnimStateComp:SetEnableAbilityAO(true)
        end
	end
end

function Ability_Task_SendYawPitchToTargetToABP:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.SendYawPitchToTargetToABP)
end

function Ability_Task_SendYawPitchToTargetToABP:IsLocalCharacter(Context)
    if _SPFeature.Utils.WorldUtils:IsStandalone() then
        return true
    end
    local Character = Context:GetOwner()
    if Character then
        return Character:IsLocallyControlled()
    end
    return false
end

return Ability_Task_SendYawPitchToTargetToABP