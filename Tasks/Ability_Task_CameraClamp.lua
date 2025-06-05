local Ability_Task_CameraClamp = UE4.Class(nil, "Ability_Task_CameraClamp")

function Ability_Task_CameraClamp:OnTaskStartBP(Context)
    local owner = self:GetLocalCharacter(Context)
    if owner ~= nil then
        local world = self:GetWorld()
        local cameraManager = UE4.UMoeCameraManagerSubSystem.GetInstance()
        self.CameraMainPlayer = cameraManager:GetMoeCameraMainPlayer(world)

        local _, cameraRot = UE4.USPGameLibrary.GetCameraLocationAndRotation()
        self.CameraMainPlayer:AddSPPitchParam(self.CameraState, UE4.FVector2D(-self.PitchClamp, self.PitchClamp))
        self.CameraMainPlayer:AddSPYawParam(self.CameraState, cameraRot.Yaw - self.YawClamp, cameraRot.Yaw + self.YawClamp)
        self.CameraMainPlayer:EnterCameraPitchClampState(self.CameraState)
        self.CameraMainPlayer:EnterCameraYawClampState(self.CameraState)
    end
end

function Ability_Task_CameraClamp:OnTaskEndBP(Context)
    if self.CameraMainPlayer ~= nil then
        self.CameraMainPlayer:CleanSPPitchParam(self.CameraState)
        self.CameraMainPlayer:CleanSPYawParam(self.CameraState)
        self.CameraMainPlayer:LeaveCameraPitchClampState(self.CameraState)
        self.CameraMainPlayer:LeaveCameraYawClampState(self.CameraState)
    end
end

function Ability_Task_CameraClamp:GetLocalCharacter(Context)
    local owner = Context:GetOwner()
    if _SPFeature.Utils.WorldUtils:IsStandalone() then
        return owner
    end
    if owner:IsLocallyControlled() == true then
        return owner
    end
    return nil
end

return Ability_Task_CameraClamp