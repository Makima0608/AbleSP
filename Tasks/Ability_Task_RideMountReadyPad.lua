--[[--
* @Description: 骑乘上马准备的实例数据
* @Author:      smbintang
* @DateTime:    2024/11/23
]]

---@type SPListUtils
local S_ListUtils = require("Feature.StarP.Script.Utils.ListUtils")
local S_setmetatable = setmetatable
local S_UE4 = UE4
local S_ListUtils_Clear = S_ListUtils.Clear



---@class SPAbility_Task_RideMountReadyPad
---@field CallbackOnRidePhaseChange table
---@field RideSys table
---@field Interrupted bool
local Ability_Task_RideMountReadyPad = UE4.Class(nil, "Ability_Task_RideMountReadyPad")

---@param InContext UAbleAbilityContext
---@param InListen bool
---@param InOwner UObject 必须有效
---@param InFunc? fun(InSelf:any, InContext:UAbleAbilityContext, InPad:Ability_Task_RideMountReadyPad, InSysLogic:USPRideSystemInterface, InSysObject:UObject, InEventData:FSPRideSysPhaseChangeEventData) 必须有效
function Ability_Task_RideMountReadyPad:SetEventOnRidePhaseChange(InContext, InListen, InOwner, InFunc)
    local sysLogic, sysData = self:GetRideSys()

    if sysLogic == nil or sysData == nil then
        return
    end

    local oldListen = self.CallbackOnRidePhaseChange ~= nil

    if oldListen == InListen then
        return
    end

    if InListen then
        self.CallbackOnRidePhaseChange = S_setmetatable({ InContext, InOwner, InFunc }, { __mode = "v" })
        sysLogic:ListenPhaseChangeEvent(sysData, { self, self.OnEventRidePhaseChange })
    else
        S_ListUtils_Clear(self.CallbackOnRidePhaseChange)
        self.CallbackOnRidePhaseChange = nil
        sysLogic:UnlistenPhaseChangeEvent(sysData, { self, self.OnEventRidePhaseChange })
    end
end

---@param InSysLogic USPRideSystemInterface
---@param InSysData UObject
---@param InEventData FSPRideSysPhaseChangeEventData
function Ability_Task_RideMountReadyPad:OnEventRidePhaseChange(InSysLogic, InSysData, InEventData)
    local cb = self.CallbackOnRidePhaseChange
    local context, owner, func = cb[1], cb[2], cb[3]

    if owner and func and context then
        func(owner, context, self, InSysLogic, InSysData, InEventData)
    end
end

---@param InSysLogic USPRideSystemInterface
---@param InSysData UObject
function Ability_Task_RideMountReadyPad:CacheRideSys(InSysLogic, InSysData)
    local rideSys = self.RideSys

    if rideSys == nil then
        rideSys = S_setmetatable({}, { __mode = "v" })
        self.RideSys = rideSys
    end

    rideSys[1] = InSysLogic
    rideSys[2] = InSysData
end

---@return USPRideSystemInterface, UObject
function Ability_Task_RideMountReadyPad:GetRideSys()
    local rideSys = self.RideSys

    if rideSys ~= nil then
        return rideSys[1], rideSys[2]
    else
        return nil, nil
    end
end

return Ability_Task_RideMountReadyPad
