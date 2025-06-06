---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by ecoliwu.
--- DateTime: 2024/7/16 16:26
---
local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local Ability_Task_BoneTurnTo = UE4.Class(nil, "Ability_Task_BoneTurnTo")

function Ability_Task_BoneTurnTo:OnTaskStartBP(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    -- Get Target
    local Target = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_Self)
    if not Target then
        return
    end
    -- Get Able Target
    -- local ableTarget = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_TargetActor)
    -- Temp Doing 拿主角当目标
    local ableTarget = UE4.USPGameLibrary.GetLocalPlayerCharacter()
    local meshComp = Target:GetComponentByClass(UE4.USkeletalMeshComponent.StaticClass())
    if ableTarget and meshComp and self.SocketName ~= "" then
        local targetRotation = self:GetTargetRotation(meshComp, ableTarget, Target)
        local animInstance = meshComp:GetAnimInstance()
        ScratchPad.AnimInstance = animInstance
        if self.bBlend then
            ScratchPad.TurnTarget = Target
            ScratchPad.TargetRotation = targetRotation
            ScratchPad.AbleTarget = ableTarget
            ScratchPad.MeshComp = meshComp
            ScratchPad.OriginRotation = meshComp:GetSocketRotation(self.SocketName)
            ScratchPad.OriginYaw = animInstance[self.AnimInstanceYawParamName]
        else
            if self.AnimInstanceYawParamName ~= "" and self.bSetYaw then
                if UE4.UKismetMaterialLibrary.IsValid(animInstance) then
                    animInstance[self.AnimInstanceYawParamName] = targetRotation.Yaw
                end
            end
            if self.AnimInstancePitchParamName ~= "" and self.bSetPitch then
                if UE4.UKismetMaterialLibrary.IsValid(animInstance) then
                    animInstance[self.AnimInstancePitchParamName] = targetRotation.Pitch
                end
            end
        end
    end
end

function Ability_Task_BoneTurnTo:GetTargetRotation(meshComp, target, turnToOwner)
    local socketRotation = meshComp:GetSocketRotation(self.SocketName)
    local Rot = UE4.UKismetMathLibrary.FindLookAtRotation(meshComp:GetSocketLocation(self.SocketName), target:K2_GetActorLocation())
    local Yaw = self.bSetYaw and Rot.Yaw or socketRotation.Yaw
    local Pitch = self.bSetPitch and Rot.Pitch or socketRotation.Pitch
    local targetRotation = UE4.FRotator(Pitch, Yaw, socketRotation.Roll)
    local dir = turnToOwner:GetActorRightVector()
    targetRotation = targetRotation - UE4.UKismetMathLibrary.Conv_VectorToRotator(dir) + self.OffsetRotation
    return targetRotation
end

function Ability_Task_BoneTurnTo:OnTaskTickBP(Context, DeltaTime)
    if not self.bBlend then
        return
    end
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    ScratchPad.ElapsedTime = ScratchPad.ElapsedTime + DeltaTime
    local targetRotation
    if self.bFollowTarget then
        targetRotation = self:GetTargetRotation(ScratchPad.MeshComp, ScratchPad.AbleTarget, ScratchPad.TurnTarget)
    else
        targetRotation = ScratchPad.TargetRotation
    end
    local DurationTime = self:GetDuration()
    local Progress = DurationTime > 0 and (ScratchPad.ElapsedTime / DurationTime) or 0
    local Alpha = self.Curve and self.Curve:GetFloatValue(Progress) or 1
    if self.AnimInstanceYawParamName and self.bSetYaw then
        if UE4.UKismetMaterialLibrary.IsValid(ScratchPad.AnimInstance) then
            ScratchPad.AnimInstance[self.AnimInstanceYawParamName] =  UE4.UKismetMathLibrary.Lerp(ScratchPad.OriginYaw, targetRotation.Yaw, Alpha)
        end
    end
    if self.AnimInstancePitchParamName and self.bSetPitch then
        if UE4.UKismetMaterialLibrary.IsValid(ScratchPad.AnimInstance) then
            ScratchPad.AnimInstance[self.AnimInstancePitchParamName] =  UE4.UKismetMathLibrary.Lerp(ScratchPad.OriginRotation.Pitch, targetRotation.Pitch, Alpha)
        end
    end
end

function Ability_Task_BoneTurnTo:OnTaskEndBP(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    self:ResetScratchPadBP(ScratchPad)
end

function Ability_Task_BoneTurnTo:ResetScratchPadBP(ScratchPad)
    if ScratchPad then
        ScratchPad.TurnTarget = nil
        ScratchPad.TargetRotation = nil
        ScratchPad.ElapsedTime = 0
        ScratchPad.AnimInstance = nil
        ScratchPad.AbleTarget = nil
        ScratchPad.MeshComp = nil
        ScratchPad.OriginRotation = nil
        ScratchPad.OriginYaw = nil
    end
end

function Ability_Task_BoneTurnTo:IsSingleFrameBP()
    return not self.bBlend
end

return Ability_Task_BoneTurnTo