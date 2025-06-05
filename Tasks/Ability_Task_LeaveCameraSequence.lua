local Ability_Task_LeaveCameraSequence = UE4.Class(nil, "Ability_Task_LeaveCameraSequence")

function Ability_Task_LeaveCameraSequence:OnTaskStartBP(Context)
    self.SequenceEnd = UE4.USPGameCameraUtils.IsSequenceEnd()
    if self.SequenceEnd then
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
    if self.LeaveOnTaskStart then
        UE4.USPGameCameraUtils.LeaveSequenceCameraState()
    end
end

function Ability_Task_LeaveCameraSequence:OnTaskTickBP(Context, DeltaTime)
    if not self.SequenceEnd and not self.LeaveOnTaskStart then
        self.SequenceEnd = UE4.USPGameCameraUtils.IsSequenceEnd()

        if self.SequenceEnd then
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

function Ability_Task_LeaveCameraSequence:OnTaskEndBP(Context)
    if not self.LeaveOnTaskStart then
        UE4.USPGameCameraUtils.LeaveSequenceCameraState()
    end
end

return Ability_Task_LeaveCameraSequence