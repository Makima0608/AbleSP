local Ability_Task_UpdateUIWhenCharged = UE4.Class(nil, "Ability_Task_UpdateUIWhenCharged")

function Ability_Task_UpdateUIWhenCharged:OnTaskStartBP(Context)
    _SP.Log("SPAbilityTask", "Ability_Task_UpdateUIWhenCharge:OnTaskStartBP")
    --local bIsChargeEnable = self.bEnableCharge
    --local fullChargeTime = self.FullChargeTime
    self.PastTime = 0
    self.CurrentColor = nil
    self.ChargeStageConfigTbl = {}
    local temp = self.ChargeStageConfigMap and self.ChargeStageConfigMap:ToTable()
    if temp and next(temp) then
        for floatKey, linearColorValue in pairs(temp) do
            table.insert(self.ChargeStageConfigTbl, {floatKey = floatKey, linearColorValue = linearColorValue})
        end
        table.sort(self.ChargeStageConfigTbl, function(a, b)
            return a.floatKey < b.floatKey
        end)
    end
    --debug
    if next(self.ChargeStageConfigTbl) then
        for i, v in ipairs(self.ChargeStageConfigTbl) do
            _SP.Log("UpdateUIWhenCharged", "ChargeStageConfig  v.floatKey:", v.floatKey, "/ v.linearColorValue:", v.linearColorValue)
        end
    else
        _SP.Log("UpdateUIWhenCharged", "NO  ChargeStageConfig")
    end
end

function Ability_Task_UpdateUIWhenCharged:OnTaskEndBP(Context)
    self.PastTime = nil
    self.CurrentColor = nil
    self.ChargeStageConfigTbl = nil
    _SP.Log("SPAbilityTask", "Ability_Task_UpdateUIWhenCharge:OnTaskEndBP")
end

function Ability_Task_UpdateUIWhenCharged:OnTaskTickBP(Context, DeltaTime)
    self.PastTime = self.PastTime + DeltaTime
    if next(self.ChargeStageConfigTbl) then
        for i, v in ipairs(self.ChargeStageConfigTbl) do
            if v.floatKey and self.PastTime <= v.floatKey then
                self.CurrentColor = v.linearColorValue
                break
            end
        end
    end
    --debug
    --if self.CurrentColor then
    --    _SP.Log("UpdateUIWhenCharged", "PastTime: "..self.PastTime, "|  CurrentColor:", self.CurrentColor.R.." / "..self.CurrentColor.G.." / "..self.CurrentColor.B)
    --else
    --    _SP.Log("UpdateUIWhenCharged", "PastTime: "..self.PastTime, "|  No CurrentColor")
    --end
    local data = {}
    data.PastTime = self.PastTime
    data.FullChargeTime = self.FullChargeTime
    data.CurrentColor = self.CurrentColor
    _SPFeature.EventManager:DispatchEvent(_SP.EventEnum.ON_ABILITY_TASK_CHARGE_UPDATE, data)
    --_SP.Log("SPAbilityTask", "Ability_Task_UpdateUIWhenCharge:OnTaskTickBP")
end

function Ability_Task_UpdateUIWhenCharged:GetTaskRealmBP()
    return UE4.EAbleAbilityTaskRealm.ATR_Client
end

return Ability_Task_UpdateUIWhenCharged