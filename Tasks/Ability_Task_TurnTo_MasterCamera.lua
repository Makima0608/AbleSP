local Ability_Task_TurnTo_MasterCamera = UE4.Class(nil, "Ability_Task_TurnTo_MasterCamera")

local WeaponRange = 100000
function Ability_Task_TurnTo_MasterCamera:OnTaskStartBP(Context)
    local TargetArray = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, TargetArray)
    for i = 1, TargetArray:Length() do
        local TargetActor = TargetArray:Get(i)

        local CameraRotation = self:GetTurnToRotaion(TargetActor)
        self:TurnToSetPetRotation(TargetActor, CameraRotation)
    end
end

function Ability_Task_TurnTo_MasterCamera:OnTaskTickBP(Context, DeltaTime)
    local TargetActor = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_Self)

    local cameraRotation = self:GetTurnToRotaion(TargetActor)
    if not cameraRotation then
        return
    end

    local actorRotation = TargetActor:K2_GetActorRotation()
    local targetRotaion = cameraRotation
    if self.LockPitch then
        targetRotaion.Pitch = actorRotation.Pitch
    end
    if self.LockRoll then
        targetRotaion.Roll = actorRotation.Roll
    end
    if self.LockYaw then
        targetRotaion.Yaw = actorRotation.Yaw
    end

    local newRotation = UE4.UKismetMathLibrary.RInterpTo(actorRotation, targetRotaion, DeltaTime, self.RotateSpeed)

    self:TurnToSetPetRotation(TargetActor, newRotation)
end

function Ability_Task_TurnTo_MasterCamera:TurnToSetPetRotation(PetActor, Rotation)
    if PetActor.SPSetActorRotation then
        PetActor:SPSetActorRotation(Rotation)
    end
end

---获取啾灵主人的摄像机
---@param PetActor ASPGameMonsterBase
---@return PlayerCameraManager
function Ability_Task_TurnTo_MasterCamera:GetMasterCameraManager(PetActor)
    if not _SP.IsValid(PetActor) then
        return nil
    end

    --- 仅支持Monster的转向
    if not PetActor:IsA(UE4.ASPGameMonsterBase) then
        return nil
    end

    local masterUID = PetActor:GetOwnerPlayerUID()
    local playerController = UE4.UMoeGameLibrary.GetPlayerControllerByPlayerUID(masterUID)
    if not playerController then
        return nil
    end

    return playerController.PlayerCameraManager
end

---获取转向旋转
---@param PetActor ASPGameMonsterBase
---@return FRotator
function Ability_Task_TurnTo_MasterCamera:GetTurnToRotaion(PetActor)
    local NewRotation = nil
    if self.UseCrosshairDirection then
        NewRotation = self:GetWeaponCameraRotation(PetActor)
    else
        NewRotation = self:GetPlayerAimRotaion(PetActor)
    end
    return NewRotation
end

---获取啾灵主人的摄像机朝向
---@param PetActor ASPGameMonsterBase
---@return FRotator
function Ability_Task_TurnTo_MasterCamera:GetPlayerAimRotaion(PetActor)
    -- if not _SP.IsValid(PetActor) then
    --     return nil
    -- end

    -- --- 仅支持Monster的转向
    -- if not PetActor:IsA(UE4.ASPGameMonsterBase) then
    --     return nil
    -- end

    -- local MasterCharacter = PetActor:GetMasterInCombatTeam()
    -- if not _SP.IsValid(MasterCharacter) then
    --     return nil
    -- end

    -- 接口"GetAimingDirection"仅支持客户端
    -- local CharacterForward = MasterCharacter.Mesh:GetRightVector()
    -- local TurnToDirection = CharacterForward
    -- local SpawnLocation = MasterCharacter:K2_GetActorLocation()
    -- TurnToDirection = UE4.USPGameLibrary.GetAimingDirection(PetActor, SpawnLocation, MasterCharacter, UE4.ESPTraceType.Weapon, 10000, CharacterForward)
    -- local NewRotation = UE4.UKismetMathLibrary.Conv_VectorToRotator(TurnToDirection)

    -- return NewRotation

    local playerCameraManager = self:GetMasterCameraManager(PetActor)
    if not _SP.IsValid(playerCameraManager) then
        return nil
    end
    return playerCameraManager:GetCameraRotation()
end

---获取武器准心瞄准的朝向
---@param PetActor ASPGameMonsterBase
---@return FRotator
function Ability_Task_TurnTo_MasterCamera:GetWeaponCameraRotation(PetActor)
    if not _SP.IsValid(PetActor) then
        return nil
    end

    --- 仅支持Monster的转向
    if not PetActor:IsA(UE4.ASPGameMonsterBase) then
        return nil
    end

    local MasterCharacter = PetActor:GetMasterInCombatTeam()
    if not _SP.IsValid(MasterCharacter) then
        return nil
    end

    -- 接口"GetCrossHairDirection"仅支持客户端
    -- local CharacterForward = MasterCharacter.Mesh:GetRightVector()
    -- local TurnToDirection = CharacterForward
    -- local SpawnLocation = MasterCharacter:K2_GetActorLocation()
    -- TurnToDirection = UE4.USPGameLibrary.GetCrossHairDirection(PetActor, SpawnLocation, MasterCharacter, UE4.ESPTraceType.Weapon, CharacterForward)
    -- local NewRotation = UE4.UKismetMathLibrary.Conv_VectorToRotator(TurnToDirection)

    -- return NewRotation

    local CrtWeapon = MasterCharacter:GetCrtEquipWeapon()
    if not _SP.IsValid(CrtWeapon) then
        return nil
    end

    local StartPos = MasterCharacter:K2_GetActorLocation()
    --- 使用武器的相机计算瞄准方向
    local CameraLocation = CrtWeapon.CameraLocation
    local CameraRotation = CrtWeapon.CameraRotation
    if not CameraLocation or not CameraRotation then
        return nil
    end
    local CameraEndPosition = CameraLocation + CameraRotation:GetForwardVector() * WeaponRange
    local DirToEnd = UE4.UKismetMathLibrary.Normal(CameraEndPosition - StartPos)
    local Rotation = UE4.UKismetMathLibrary.Conv_VectorToRotator(DirToEnd)
    return Rotation
end

function Ability_Task_TurnTo_MasterCamera:OnTaskEndBP(Context, Result)
    local TargetActor = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_Self)
    if not _SP.IsValid(TargetActor) then
        return
    end

    --- 结束时可选择是否恢复俯仰角
    if self.RestorePitch then
        local petRotation = TargetActor:K2_GetActorRotation()
        petRotation.Pitch = 0
        self:TurnToSetPetRotation(TargetActor, petRotation)
    end
end

return Ability_Task_TurnTo_MasterCamera