local FuncLib = require("Feature.StarP.Script.System.SPWidgetFunctionLibrary")
local SPGameUIConst = require("Feature.StarP.Script.System.SPGameUIConst")
local SpBtnCfg = SPGameUIConst.ButtonConfig
local SpBtnType = SPGameUIConst.ButtonType
local ButtonType = UE4.ESPUIButtonType
local ButtonState = UE4.ESPUIButtonState
local NoneButtonType = UE4.ESPUIButtonType.None

local Ability_Task_ButtonState = UE4.Class(nil, "Ability_Task_ButtonState")

-- 反向查找
local function GetButtonNameForEach(value)
    for key, val in pairs(ButtonType) do
        if val == value then
            return key  -- 返回的是枚举项的名称（字符串类型）
        end
    end
    return "Unknown"
end

-- 反序列（推荐
---@param buttonType UE4.ESPUIButtonType
local function GetCurBtnCfgByInt(buttonType)
    if buttonType and buttonType > NoneButtonType then
        local SpBtnTypeName = SpBtnType[buttonType]
        if SpBtnTypeName then
            return SpBtnCfg[SpBtnTypeName]
        end
    end
end

-- 自行匹配 按钮枚举数组和执行函数
---@param buttonTbl table
---@param executeFunc function
local function ForEachBtnCfgAndExecuteFunc(buttonTbl, executeFunc)
    if buttonTbl and next(buttonTbl) then
        for _,value in pairs(buttonTbl) do
            local cfg = GetCurBtnCfgByInt(value)
            if cfg then
                executeFunc(cfg)
            end
        end
    end
end

---@param Context UAblAbilityContext
function Ability_Task_ButtonState:OnTaskStartBP(Context)
    self.DisabledTbl = self.DisabledList and self.DisabledList:ToTable()
    self.HiddenTbl = self.HiddenList and self.HiddenList:ToTable()
    self.HighlightTbl = self.HighlightList and self.HighlightList:ToTable()

    ForEachBtnCfgAndExecuteFunc(self.DisabledTbl, function(cfg)
        _SP.Client.SPButtonStateManager:SetDisableByFlag(cfg.Name, cfg.Flag.IsCausedByUIBtnStateTask, true)
    end)
    ForEachBtnCfgAndExecuteFunc(self.HiddenTbl, function(cfg)
        _SP.Client.SPButtonStateManager:SetHideByFlag(cfg.Name, cfg.Flag.IsCausedByUIBtnStateTask, true)
    end)

end


---@param Context UAblAbilityContext
function Ability_Task_ButtonState:OnTaskEndBP(Context)
    ForEachBtnCfgAndExecuteFunc(self.DisabledTbl, function(cfg)
        _SP.Client.SPButtonStateManager:SetDisableByFlag(cfg.Name, cfg.Flag.IsCausedByUIBtnStateTask, false)
    end)
    ForEachBtnCfgAndExecuteFunc(self.HiddenTbl, function(cfg)
        _SP.Client.SPButtonStateManager:SetHideByFlag(cfg.Name, cfg.Flag.IsCausedByUIBtnStateTask, false)
    end)
end

-----@param Context UAblAbilityContext
--function Ability_Task_ButtonState:FindButton(Context)
--
--end


return Ability_Task_ButtonState