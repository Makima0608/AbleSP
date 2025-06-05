local Ability_Task_SetPosNear = UE4.Class(nil, "Ability_Task_SetPosNear")
function Ability_Task_SetPosNear:ErrLog(__FUNC_NAME__, ...)
    _SP.LogError("[Ability_Task_SetPosNear]" .. __FUNC_NAME__, ...)
end

function Ability_Task_SetPosNear:OnTaskStartBP(Context)
    local ErrLog = function (...) self:ErrLog("::OnTaskStartBP", ...) end

    local OwnerActor = Context:GetOwner()
    if OwnerActor == nil then
        ErrLog("The OwnerActor is nil")
        return
    end

    local OwnerMonster = OwnerActor:Cast(UE4.ASPGameMonsterBase)
    if OwnerMonster == nil then
        ErrLog("The OwnerMonster is nil")
        return
    end

    if self.m_TaskTargets == nil then
        return
    end
    local len = self.m_TaskTargets:Length()
    for iter = 1, len do
        local TargetType = self.m_TaskTargets:Get(iter)
        local TargetActor = self:GetSingleActorFromTargetTypeBP(Context, TargetType)
        if TargetActor ~= nil and TargetActor:Cast(UE4.ASPGameCharacterBase) then
            local Pos = TargetActor:K2_GetActorLocation()
            local MeshComp = TargetActor:GetComponentByClass(UE4.USkeletalMeshComponent.StaticClass())
            if MeshComp == nil then
                return
            end
            local Rotation = MeshComp:K2_GetComponentRotation();
		    Rotation.Yaw = 90 + Rotation.Yaw;
            local forward = Rotation:ToVector()
            local targetLocation = Pos + forward * 200

            local start = targetLocation + UE4.FVector(0, 0, 1000)
            local endPoint = targetLocation + UE4.FVector(0, 0, -1000)

            local DebugTrace = UE4.EDrawDebugTrace.None
            local HitResult, bHit = UE4.UKismetSystemLibrary.LineTraceSingle(TargetActor, start, endPoint, UE4.ECollisionChannel.ECC_WorldStatic, false, nil, DebugTrace, nil, true,UE4.FLinearColor(1,1,1,1), UE4.FLinearColor(1,0,0,1),3)
            if bHit then
                local hitPoint = UE4.FVector(HitResult.ImpactPoint.X, HitResult.ImpactPoint.Y, HitResult.ImpactPoint.Z)
                local HalfHeight = OwnerMonster.CapsuleComponent:GetScaledCapsuleHalfHeight()
                OwnerMonster:K2_SetActorLocation(UE4.FVector(hitPoint.X,hitPoint.Y,hitPoint.Z + HalfHeight))
            end
        end
    end
end

function Ability_Task_SetPosNear:OnTaskEndBP(Context)
    local ErrLog = function (...) self:ErrLog("::OnTaskEndBP", ...) end

    local OwnerActor = Context:GetOwner()
    if OwnerActor == nil then
        ErrLog("The OwnerActor is nil")
        return
    end

    local OwnerMonster = OwnerActor:Cast(UE4.ASPGameMonsterBase)
    if OwnerMonster == nil then
        ErrLog("ASPGameMonsterBase is nil")
        return
    end

end

return Ability_Task_SetPosNear