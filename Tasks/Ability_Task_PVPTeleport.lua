local Ability_Task_PVPTeleport = UE4.Class(nil, "Ability_Task_PVPTeleport")
local bShowDebug = UE4.USPAbilityFunctionLibrary.IsVerboseDebug()
local ESPPVPSideType = _SPFeature.ServerEnum.SPPVPSideType

function Ability_Task_PVPTeleport:OnTaskStartBP(Context)
    print("Ability_Task_PVPTeleport:OnTaskStartBP")
    local Owner = Context:GetOwner()
    if not UE4.UKismetSystemLibrary.IsValid(Owner) then
        print("Ability_Task_PVPTeleport:Valid target position found fail : Owner isUnValid")
        return false
    end

    local TargetActor = self:GetSingleActorFromTargetTypeBP(Context, self.AbilityTargetType)
    if not UE4.UKismetSystemLibrary.IsValid(TargetActor) then
        print("Ability_Task_PVPTeleport TargetActor is not Valid")
        return
    end
    local X, Y, DesiredTargetLocation = UE4.USPPvpHexagonCubicManager.GetInstance():GetTargetNearestPositionUseByAssn(TargetActor)
    Owner:K2_SetActorLocation(DesiredTargetLocation, false, nil, false)
    local Entity = self:GetMonsterManager():GetMonsterEntityByMonsterBase(Owner)
    if Entity ~= nil then
        UE4.USPPvpHexagonCubicManager.GetInstance():SetHexagonCubicLocked(Entity:GetPlayerUID(), Entity:GetItemInstId(), X, Y)
    end

    -- 让刺客朝向目标
    local LocationOwner = Owner:K2_GetActorLocation()
    local LocationTarget = TargetActor:K2_GetActorLocation()

    local Direction = LocationTarget - LocationOwner
    Direction:Normalize()

    -- 计算目标旋转
    local TargetRotation = UE4.UKismetMathLibrary.MakeRotFromX(Direction)

    -- 设置Owner旋转
    Owner:K2_SetActorRotation(TargetRotation)
end

function Ability_Task_PVPTeleport:GetMonsterManager()
    if _SP.IsDS then
        return _SP.DS.MonsterPVPManager
    else
        return _SP.Client.MonsterPVPManager
    end
end

return Ability_Task_PVPTeleport