local SPAbilityClasses = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local Ability_Task_CameraSequence = UE4.Class(nil, "Ability_Task_CameraSequence")

local ESPAbilityCameraType = SPAbilityUtils.LoadEnum(SPAbilityClasses.ESPAbilityCameraType)
local GetCameraTransformDataKeyFunc = {
    [ESPAbilityCameraType.BattleTalentUltimate] = function (self, Context)
        local character = Context:GetOwner()
        local pet = character and character:GetUnleashedPet()
        local petTypeConfig = pet and pet:GetMonsterTypeConfig()
        local petBodyType = petTypeConfig and petTypeConfig.bodyType
        return petBodyType and tostring(petBodyType)
    end,
}

function Ability_Task_CameraSequence:OnTaskStartBP(Context)
    self.SequenceEnd = false

    local transformDataKey = GetCameraTransformDataKeyFunc[self.CameraType](self, Context)
    local transformData = transformDataKey and self.SequenceDataMap:Find(transformDataKey)
    if not transformData then
        transformData = self.SequenceDataMap:Find("Default")
    end
    if not transformData then
        _SP.LogWarning("Ability_Task_CameraSequence invalid transformData", "transformDataKey:", transformDataKey)
        return
    end

    local _, cameraRot = UE4.USPGameLibrary.GetCameraLocationAndRotation()
    local baseRotator = UE4.FRotator(0, cameraRot.Yaw, 0)
    if self.IsUseCharacterRotation then
        local meshComp = Context:GetOwner():GetComponentByClass(UE4.USkeletalMeshComponent:StaticClass())
        local meshRotator = meshComp:K2_GetComponentRotation()
        baseRotator = UE4.FRotator(0, meshRotator.Yaw + 90, 0)
    end
    local baseQuat = baseRotator:ToQuat()
    local baseLocation = Context:GetOwner():K2_GetActorLocation()
    local baseTransform = UE4.FTransform(baseQuat, baseLocation)

    --默认是false，做一个socket检查，如果没有按false处理
    local CurBlendToEnd = false
    local unleashedPet = Context:GetOwner() and Context:GetOwner():GetUnleashedPet()
    if unleashedPet then
        local talentSocketName = "Talent_003"
        local meshComp = unleashedPet:GetComponentByClass(UE4.USkeletalMeshComponent.StaticClass())
        if meshComp and meshComp:DoesSocketExist(talentSocketName) then
            CurBlendToEnd = self.BlendToEndParam
        end
    end
    UE4.USPGameCameraUtils.SwitchToSequenceCameraState(transformData, baseTransform, CurBlendToEnd, self.BlendEndParamTime, self.InputRotationType, self.TargetAllowRotTime)
end

function Ability_Task_CameraSequence:OnTaskEndBP(Context)
    if not self.LeaveStateOnEnd then
        return
    end

    self.SequenceEnd = false
    UE4.USPGameCameraUtils.LeaveSequenceCameraState()
end

function Ability_Task_CameraSequence:OnTaskTickBP(Context, DeltaTime)
    if not self.SequenceEnd then
        self.SequenceEnd = UE4.USPGameCameraUtils.IsSequenceEnd()

        if self.BlendToEndParam and self.SequenceEnd then
            local character = Context:GetOwner()
            local unleashedPet = character and character:GetUnleashedPet()
            if unleashedPet then
                local talentSocketName = "Talent_003"
                local meshComp = unleashedPet:GetComponentByClass(UE4.USkeletalMeshComponent.StaticClass())
                if meshComp and meshComp:DoesSocketExist(talentSocketName) then
                    local socketTransform = meshComp:GetSocketTransform(talentSocketName, UE4.ERelativeTransformSpace.RTS_Component)
                    local meshTransform = meshComp:K2_GetComponentToWorld()
                    UE4.USPGameCameraUtils.SetEndTransform(meshTransform, socketTransform, unleashedPet.MonsterConfig.typeId)
                end
            end
        end
    end
end

return Ability_Task_CameraSequence