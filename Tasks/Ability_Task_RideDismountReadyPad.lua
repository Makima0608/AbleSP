--[[--
* @Description: 骑乘下马的实例数据
* @Author:      smbintang
* @DateTime:    2024/11/23
]]

local S_setmetatable = setmetatable
local S_UE4 = UE4



---@class SPAbility_Task_RideDismountReadyPad
---@field RideSys table
local Ability_Task_RideDismountReadyPad = UE4.Class(nil, "Ability_Task_RideDismountReadyPad")

---@param InSysLogic USPRideSystemInterface
---@param InSysData UObject
function Ability_Task_RideDismountReadyPad:CacheRideSys(InSysLogic, InSysData)
    local rideSys = self.RideSys

    if rideSys == nil then
        rideSys = S_setmetatable({}, { __mode = "v" })
        self.RideSys = rideSys
    end

    rideSys[1] = InSysLogic
    rideSys[2] = InSysData
end

---@return USPRideSystemInterface, UObject
function Ability_Task_RideDismountReadyPad:GetRideSys()
    local rideSys = self.RideSys

    if rideSys ~= nil then
        return rideSys[1], rideSys[2]
    else
        return nil, nil
    end
end

return Ability_Task_RideDismountReadyPad
