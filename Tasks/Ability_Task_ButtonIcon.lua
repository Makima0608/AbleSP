
local Ability_Task_ButtonIcon = UE4.Class(nil, "Ability_Task_ButtonIcon")
local SPGameUIConst = require("Feature.StarP.Script.System.SPGameUIConst")
local SpBtnCfg = SPGameUIConst.ButtonConfig
local SpBtnType = SPGameUIConst.ButtonType
local NoneButtonType = UE4.ESPUIButtonType.None

---@param Context UAblAbilityContext
function Ability_Task_ButtonIcon:OnTaskStartBP(Context)
    if self:IsLocalCharacter(Context) then
        if self.ButtonIconMap then
            local buttonList = self.ButtonIconMap:ToTable()
            if buttonList and next(buttonList) then
                for buttonType, icon in pairs(buttonList) do
                    if buttonType > NoneButtonType and icon then
                        local SpBtnTypeName = SpBtnType[buttonType]
                        if SpBtnTypeName then
                            local SpBtn = SpBtnCfg[SpBtnTypeName]
                            if SpBtn then
                                self:ChangeButtonIcon(SpBtn.Name, icon)
                            end
                        end
                    end
                end
            end
        end
    end
end

function Ability_Task_ButtonIcon:ChangeButtonIcon(btnName, btnIcon)
    if btnName then
        _SP.Client.SPButtonStateManager:ChangeButtonIcon(btnName, btnIcon)
    end
end

---@param Context UAblAbilityContext
function Ability_Task_ButtonIcon:OnTaskEndBP(Context)
    if self:IsLocalCharacter(Context) then
        if self.ButtonIconMap then
            local buttonList = self.ButtonIconMap:ToTable()
            if buttonList and next(buttonList) then
                for buttonType, icon in pairs(buttonList) do
                    if buttonType > NoneButtonType and icon then
                        local SpBtnTypeName = SpBtnType[buttonType]
                        if SpBtnTypeName then
                            local SpBtn = SpBtnCfg[SpBtnTypeName]
                            if SpBtn then
                                self:ResetButtonIcon(SpBtn.Name)
                            end
                        end
                    end
                end
            end
        end
    end
end

function Ability_Task_ButtonIcon:ResetButtonIcon(btnName)
    if btnName then
        _SP.Client.SPButtonStateManager:ResetButtonIcon(btnName)
    end
end

function Ability_Task_ButtonIcon:IsLocalCharacter(Context)
    if _SPFeature.Utils.WorldUtils:IsStandalone() then
        return true
    end
    local Character = Context:GetOwner()
    if Character then
        return Character:IsLocallyControlled()
    end
    return false
end

return Ability_Task_ButtonIcon