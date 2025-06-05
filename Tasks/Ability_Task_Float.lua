local SPBattleTalentUtil = require("Feature.StarP.Script.System.BattleTalent.SPBattleTalentUtil")
local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local Ability_Task_Float = UE4.Class(nil, "Ability_Task_Float")

function Ability_Task_Float:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.Float)
end

function Ability_Task_Float:OnTaskStartBP(Context)
    _SP.Log("Ability_Task_Float", "EnterFloatState")
    if self.SegmentConfig:Length() == 0 then
        _SP.Log("Ability_Task_Float", "get nil config")
        return
    end

    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    local Target = self:GetSingleActorFromTargetTypeBP(Context, self.AbilityTargetType)
    if not _SP.IsValid(Target) then
        return
    end
    --拿到able当前运行到的segment的配置
    --0.hacking首先在開始曲线前就把人物状态改成配置状态
    local CharMovementComponent = Target.CharacterMovement
    if CharMovementComponent then
        CharMovementComponent:SetMovementMode(self.RecoverMovementMode)
        if self.bClearVelocity then
            CharMovementComponent:StopMovementImmediately()
        end
    end
    --1.从Context拿到able当前运行到了哪个segment
    local CurSegmentId = Context:GetActiveSegmentIndex()
    --2.拿到对应的配置
    local CurConfigId,CurConfig = self:FindCorrespondConfigAndId(CurSegmentId)
    ScratchPad.CurConfigId = CurConfigId
    self:ApplyNewConfig(CurConfig,Context,ScratchPad)
end

function Ability_Task_Float:OnTaskTickBP(Context, DeltaTime)
    --tick检查当前able运行的segment是否与task中使用的configid匹配，若不匹配，说明able已经切换了segment，需要abort掉之前的曲线handle，并开启一条新的
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        _SP.Log("[Test]Ability_Task_Float", "ScratchPad is nil")
        return
    end

    if ScratchPad.CurConfigId ~= Context:GetActiveSegmentIndex() then
        _SP.Log("Ability_Task_Float", "detect segment switch,new segment id is",Context:GetActiveSegmentIndex())
        if ScratchPad.Handle then
            ScratchPad.Handle:Abort()
            ScratchPad.Handle = nil
        end

        local CurConfigId,CurConfig = self:FindCorrespondConfigAndId(Context:GetActiveSegmentIndex())
        ScratchPad.CurConfigId = CurConfigId
        self:ApplyNewConfig(CurConfig,Context,ScratchPad)
    end
end

function Ability_Task_Float:OnTaskEndBP(Context, Result)
    _SP.Log("Ability_Task_Float", "ExitFloatState")
    local Target = self:GetSingleActorFromTargetTypeBP(Context, self.AbilityTargetType)
    if _SP.IsValid(Target) then
        Target:RecoverMovement(true)
        local CharMovementComponent = Target.CharacterMovement
        if CharMovementComponent then
            local CurMovementMode = CharMovementComponent.MovementMode
            if CurMovementMode == UE4.EMovementMode.MOVE_Custom then
                CharMovementComponent:SetMovementMode(UE4.EMovementMode.MOVE_Walking)
            end
        end
    end
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    if ScratchPad.Handle then
        _SP.Log("[Test]Ability_Task_Float", "EndTask Handle Abort")
        ScratchPad.Handle:Abort()
        ScratchPad.Handle = nil
    end
end

function Ability_Task_Float:FindCorrespondConfigAndId(InCurSegmentIndex)
    local CorrespondConfigId,CorrespondConfig
    for i = 1,self.SegmentConfig:Length() do
        local TempConfig = self.SegmentConfig:Get(i)
        if TempConfig.CurSegmentIndex == InCurSegmentIndex then
            CorrespondConfig = TempConfig
            CorrespondConfigId = TempConfig.CurSegmentIndex
        end
    end
    return CorrespondConfigId,CorrespondConfig
end

function Ability_Task_Float:ApplyNewConfig(CurConfig,Context,ScratchPad)
    if CurConfig == nil then
        _SP.Log("Ability_Task_Float", "lack current segment config, current segment index is:",Context:GetActiveSegmentIndex())
        return
    end
    local Target = self:GetSingleActorFromTargetTypeBP(Context, self.AbilityTargetType)
    local FloatOffset = CurConfig.FloatOffset
    -- Add UltimateOffset
    if CurConfig.UseUltimateOffset == true then
        local UltimateOffset = SPBattleTalentUtil.GetYSYRelativePos(Target:GetUnleashedPet())
        local masterMeshComp = Target:GetComponentByClass(UE4.USkeletalMeshComponent.StaticClass())
        if UltimateOffset ~= nil and masterMeshComp ~= nil  then
            local targetLocation = masterMeshComp:K2_GetComponentLocation()
            UltimateOffset.X = 0
            UltimateOffset.Y = 0
            UltimateOffset.Z = UltimateOffset.Z - targetLocation.Z
            FloatOffset = UltimateOffset
        end
    end
    if _SP.IsValid(Target) then
        -- local FloatDistance = FloatOffset:Size()
        -- local distanceH = FloatOffset:Size2D()
        local distanceV = FloatOffset.Z
        local FloatDir = FloatOffset
        FloatDir:Normalize()

        local speedH = CurConfig.Speed * math.sqrt(FloatDir.X * FloatDir.X + FloatDir.Y * FloatDir.Y)
        local speedV = CurConfig.Speed * FloatDir.Z

        local accelerationH = CurConfig.Acceleration * math.sqrt(FloatDir.X * FloatDir.X + FloatDir.Y * FloatDir.Y)
        local accelerationV = CurConfig.Acceleration * FloatDir.Z

        local IgnoreMoveDirRotationPitch = CurConfig.IgnoreRotationPitch
        local IgnoreMoveDirRotationYaw = CurConfig.IgnoreRotationYaw
        local IgnoreMoveDirRotationRoll = CurConfig.IgnoreRotationRoll

        local config = UE4.USPCharacterMovementLibrary.GetMovementConfigV1(true, 0, 0, 0, IgnoreMoveDirRotationPitch, IgnoreMoveDirRotationYaw, IgnoreMoveDirRotationRoll)
        config.bIsUseBlockingHitUpdate = false

        local processTime = CurConfig.bUseTaskTime and self:GetDuration() or CurConfig.Time
        if CurConfig.bUseCurve then
            local params = UE4.USPCharacterMovementLibrary.GetSpeedCurveParameter(nil, 0, CurConfig.VSpeedCurve, CurConfig.VSpeedScale, processTime)
            local handle = Target:MoveWorldDirByCurve(FloatDir, params, config)
            ScratchPad.Handle = handle
        else
            local params = UE4.USPCharacterMovementLibrary.GetDistanceParameter(speedH, accelerationH, speedV, accelerationV, true, distanceV)
            local handle = Target:MoveWorldDirByDistance(FloatDir, params, config)
            ScratchPad.Handle = handle
        end
    end
end

return Ability_Task_Float