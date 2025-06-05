local Ability_Task_HideUI = UE4.Class(nil, "Ability_Task_HideUI")

function Ability_Task_HideUI:OnTaskStartBP(Context)
    if self.IsHideAllUI then
        _SP.Client.SPUIManager:HideAllLayerInGame()
    else
        if not self.HideUINames or self.HideUINames:Length() == 0 then
            return
        end
        for i = 1, self.HideUINames:Length() do
            local UIName = self.HideUINames:Get(i)
            _SP.Log("Ability_Task_HideUI", "Hide HideUIName :", UIName)
            if UIName and _SPFeature.UIManager:IsWindowOpened(UIName) then
                if self.IsSpecialUI or UIName == "UI_SPGame_CrossHairPanelAll" then
                    _SP.EventManager:DispatchEvent(_SP.EventEnum.ON_TASK_HIDE_SPECIALUI, false)
                else
                    _SPFeature.UIManager:HideWindow(UIName)
                end
            end
        end
    end
end

function Ability_Task_HideUI:OnTaskEndBP(Context)
    if self.IsHideAllUI then
        _SP.Client.SPUIManager:ShowAllLayerInGame()
    else
        if not self.HideUINames or self.HideUINames:Length() == 0 then
            return
        end
        for i = 1, self.HideUINames:Length() do
            local UIName = self.HideUINames:Get(i)
             _SP.Log("Ability_Task_HideUI", "Show HideUIName :", UIName)
            if UIName then
                if self.IsSpecialUI or UIName == "UI_SPGame_CrossHairPanelAll" then
                    _SP.EventManager:DispatchEvent(_SP.EventEnum.ON_TASK_HIDE_SPECIALUI, true)
                else
                    _SPFeature.UIManager:ShowWindow(UIName)
                end
            end
        end
    end
end

function Ability_Task_HideUI:IsSingleFrameBP()
    return false
end

return Ability_Task_HideUI