local Ability_Task_ZoomMonsterSize = UE4.Class(nil, "Ability_Task_ZoomMonsterSize")

function Ability_Task_ZoomMonsterSize:OnTaskTickBP(Context, DeltaTime)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    local OwnerActor = Context:GetOwner()
    if not OwnerActor then
        return
    end
    ScratchPad.ElapsedTime = ScratchPad.ElapsedTime + DeltaTime
    if self.ZoomMonsterSizeCurve then
        local value = self:GetCurrentCurveValue(ScratchPad.ElapsedTime, self.ZoomMonsterSizeCurve)
        if OwnerActor.ZoomMonsterSize then
            OwnerActor:ZoomMonsterSize(value)
        end
    end
end

function Ability_Task_ZoomMonsterSize:GetCurrentCurveValue(currentTime, curve)
    local DurationTime = self:GetDuration()
    local Progress = DurationTime > 0 and (currentTime / DurationTime) or 0
    if curve:IsA(UE4.UCurveFloat.StaticClass()) then
        return curve:GetFloatValue(Progress)
    end
end

function Ability_Task_ZoomMonsterSize:OnTaskEndBP(Context)
    local OwnerActor = Context:GetOwner()
    if not _SP.IsValid(OwnerActor) or not OwnerActor:IsA(UE4.ASPGameMonsterBase) then
        return
    end
    if OwnerActor.SetDynamicMonsterBodyShapeStatus then
        if self.ZoomMonsterSizeCurve:IsA(UE4.UCurveFloat.StaticClass()) then
            local value = self.ZoomMonsterSizeCurve:GetFloatValue(1)
            if OwnerActor.ZoomMonsterSize then
                OwnerActor:ZoomMonsterSize(value)
            end
        end
        OwnerActor:SetDynamicMonsterBodyShapeStatus(self.CurrentMonsterBodyShapeStatus)
        _SP.Log("Ability_Task_ZoomMonsterSize", "OnTaskEndBP CurrentMonsterBodyShapeStatus", tostring(self.CurrentMonsterBodyShapeStatus))
    end
end

function Ability_Task_ZoomMonsterSize:ResetScratchPadBP(ScratchPad)
    if ScratchPad then
        ScratchPad.ElapsedTime = 0
    end
end

return Ability_Task_ZoomMonsterSize