--[[--
* @Description: 修改character的碰撞尺寸，即胶囊体尺寸
* @Author:      smbintang
* @DateTime:    2024/12/09
]]

---@type SPRideSystemUtils
local S_RideSysUtils = require("Feature.StarP.Script.System.Ride.SPRideSystemUtils")
local S_UE4 = UE4
local S_string_format = string.format
local S_tostring = tostring
local S_ACharacter = S_UE4.ACharacter
local S_FlushServerMovesOnClient = S_UE4.USPGameLibrary.FlushServerMovesOnClient
local S_IsValid = S_RideSysUtils.IsObjectValid
local S_GetPawnDebugString = S_RideSysUtils.GetPawnDebugString



local S_DevelopCodesEnabled = not _SPFeature.GEnv.IsShipping

---@param InFunc fun():any
---@varargany
---@return nil
local function S_DevelopLog(InFunc, ...)
end

if S_DevelopCodesEnabled then
    S_DevelopLog = function(InFunc, ...)
        InFunc(...)
    end
end

---@class Enum_SPCharCollisionSizeSourceType 蓝图定义的
---@field ClassDefaultObject int 使用Owner的蓝图配置
---@field TargetCharacter int 在目标的胶囊体尺寸基础上叠加偏移
---@field Blueprint int 通过蓝图配置指定
---@field ContextParams int 外部传入的Context参数

---@class SPAbility_Task_ChangeCharacterCollisionSize : USPAbilityTask
---@field NotFlushMovement bool 主控端是否不上传未发送的移动帧
---@field ExchangeOwnerAndTarget bool 是否调换Owner和目标。默认修改Owner的碰撞，如果该选项为true则修改目标的碰撞并把Owner作为目标
---@field SizeSourceType Enum_SPCharCollisionSizeSourceType 尺寸数据来源
---@field RadiusValueOrOffset float 胶囊体半径取值或偏移。当SizeSourceType为TargetCharacter时当偏移使用,为Blueprint时作为值来用
---@field HalfHeightValueOrOffset float 胶囊体半高取值或偏移。当SizeSourceType为TargetCharacter时当偏移使用,为Blueprint时作为值来用
---@field RadiusParamName string 外部传入的Context参数，仅当SizeSourceType为ContextParams时生效
---@field HalfHeightParamName string 外部传入的Context参数，仅当SizeSourceType为ContextParams时生效
local Ability_Task_ChangeCharacterCollisionSize = UE4.Class(nil, "Ability_Task_ChangeCharacterCollisionSize")

---@param InContext UAbleAbilityContext
function Ability_Task_ChangeCharacterCollisionSize:OnTaskStartBP(InContext)
    local func = self.ApplyFuncList[self.SizeSourceType + 1]

    if func then
        func(self, InContext)
    end
end

---@param Context UAbleAbilityContext
---@return AActor
function Ability_Task_ChangeCharacterCollisionSize.PickFirstTarget(InContext)
    return InContext.m_TargetActors:Get(1)
end

---@param InActor AActor
---@return bool
function Ability_Task_ChangeCharacterCollisionSize.CheckTargetValid(InActor)
    return S_IsValid(InActor) and InActor:IsA(S_ACharacter)
end

---@param Context UAbleAbilityContext
function Ability_Task_ChangeCharacterCollisionSize:ApplyWithClassDefaultObject(InContext)
    -- 选取目标
    ---@type ACharacter
    local targetForApply

    if self.ExchangeOwnerAndTarget then
        targetForApply = self.PickFirstTarget(InContext)
    else
        targetForApply = InContext.m_Owner
    end

    if not self.CheckTargetValid(targetForApply) then
        S_DevelopLog(function(InLa_Context)
            _SP.Log("Ability_Task_ChangeCharacterCollisionSize", "ClassDefaultObject:not valid target, selfActor",
                S_GetPawnDebugString(InLa_Context:GetSelfActor()))
        end, InContext)
        return
    end
    -- 获取CDO
    local targetClassForApply = targetForApply:GetClass()
    ---@type ACharacter
    local targetCDOForApply = (S_IsValid(targetClassForApply) and targetClassForApply:GetDefaultObject()) or nil

    if targetCDOForApply == nil then
        S_DevelopLog(function(InLa_Context, InLa_Target)
            _SP.Log("Ability_Task_ChangeCharacterCollisionSize", S_string_format(
                "ClassDefaultObject:not valid target class, selfActor %s, target %s",
                S_GetPawnDebugString(InLa_Context:GetSelfActor()),
                S_GetPawnDebugString(InLa_Target)))
        end, InContext, targetForApply)
        return
    end
    -- 修改碰撞尺寸
    local targetCapsuleForApply = targetForApply.CapsuleComponent
    local targetCDOCapsuleForApply = targetCDOForApply.CapsuleComponent

    if not targetCapsuleForApply or not targetCDOCapsuleForApply then
        S_DevelopLog(function(InLa_Context, InLa_Target)
            _SP.Log("Ability_Task_ChangeCharacterCollisionSize", S_string_format(
                "ClassDefaultObject:not valid capsule component, selfActor %s, target %s",
                S_GetPawnDebugString(InLa_Context:GetSelfActor()),
                S_GetPawnDebugString(InLa_Target)))
        end, InContext, targetForApply)
        return
    end

    local oldRadius = 0.0
    local oldHalfHeight = 0.0
    oldRadius, oldHalfHeight = targetCapsuleForApply:GetScaledCapsuleSize(oldRadius, oldHalfHeight)
    local newRadius = 0.0
    local newHalfHeight = 0.0
    newRadius, newHalfHeight = targetCDOCapsuleForApply:GetScaledCapsuleSize(newRadius, newHalfHeight)

    S_DevelopLog(function(
        InLa_Context, InLa_Target,
        InLa_OldRadius, InLa_OldHalfHeight,
        InLa_NewRadius, InLa_NewHalfHeight)
        _SP.Log("Ability_Task_ChangeCharacterCollisionSize", S_string_format(
            "ClassDefaultObject:selfActor %s, target %s, oldSize(%f,%f) -> newSize(%f,%f)",
            S_GetPawnDebugString(InLa_Context:GetSelfActor()),
            S_GetPawnDebugString(InLa_Target),
            InLa_OldRadius, InLa_OldHalfHeight,
            InLa_NewRadius, InLa_NewHalfHeight))
    end, InContext, targetForApply, oldRadius, oldHalfHeight, newRadius, newHalfHeight)

    if oldRadius ~= newRadius or oldHalfHeight ~= newHalfHeight then
        -- 上传未发送的移动帧
        if not self.NotFlushMovement then
            S_FlushServerMovesOnClient(targetForApply.CharacterMovement)
        end

        targetCapsuleForApply:SetCapsuleSize(
            newRadius,
            newHalfHeight,
            true)
    end
end

---@param Context UAbleAbilityContext
function Ability_Task_ChangeCharacterCollisionSize:ApplyWithTargetCharacter(InContext)
    -- 选取目标
    ---@type ACharacter
    local targetForApply = InContext.m_Owner
    ---@type ACharacter
    local targetForSource = self.PickFirstTarget(InContext)

    if self.ExchangeOwnerAndTarget then
        targetForApply, targetForSource = targetForSource, targetForApply
    end

    if not self.CheckTargetValid(targetForApply) or not self.CheckTargetValid(targetForSource) then
        S_DevelopLog(function(InLa_Context, InLa_Target, InLa_Source)
            _SP.Log("Ability_Task_ChangeCharacterCollisionSize", S_string_format(
                "TargetCharacter:not valid targetForApply %s or targetForSource %s, selfActor %s",
                S_GetPawnDebugString(InLa_Target),
                S_GetPawnDebugString(InLa_Source),
                S_GetPawnDebugString(InLa_Context:GetSelfActor())))
        end, InContext, targetForApply, targetForSource)
        return
    end
    -- 获取目标碰撞尺寸
    local targetCapsuleForApply = targetForApply.CapsuleComponent
    local targetCapsuleForSource = targetForSource.CapsuleComponent

    if not targetCapsuleForApply or not targetCapsuleForSource then
        S_DevelopLog(function(InLa_Context, InLa_Target, InLa_Source, InLa_TargetCapsule, InLa_SourceCapsule)
            _SP.Log("Ability_Task_ChangeCharacterCollisionSize", S_string_format(
                "TargetCharacter:not valid capsule component, targetForApply (%s,%s), targetForSource (%s,%s), selfActor %s",
                S_GetPawnDebugString(InLa_Target), S_tostring(InLa_TargetCapsule ~= nil),
                S_GetPawnDebugString(InLa_Source), S_tostring(InLa_SourceCapsule ~= nil),
                S_GetPawnDebugString(InLa_Context:GetSelfActor())))
        end, InContext, targetForApply, targetForSource, targetCapsuleForApply, targetCapsuleForSource)
        return
    end
    -- 修改碰撞尺寸
    local oldRadius = 0.0
    local oldHalfHeight = 0.0
    oldRadius, oldHalfHeight = targetCapsuleForApply:GetScaledCapsuleSize(oldRadius, oldHalfHeight)
    local baseRadius = 0.0
    local baseHalfHeight = 0.0
    baseRadius, baseHalfHeight = targetCapsuleForSource:GetScaledCapsuleSize(baseRadius, baseHalfHeight)

    S_DevelopLog(function(
        InLa_Context, InLa_Target, InLa_Source,
        InLa_OldRadius, InLa_OldHalfHeight,
        InLa_BaseRadius, InLa_BaseHalfHeight,
        InLa_Ability)
        _SP.Log("Ability_Task_ChangeCharacterCollisionSize", S_string_format(
            "TargetCharacter:selfActor %s, target %s, source %s, oldSize(%f,%f) -> baseSize(%f,%f) + offset(%f,%f)",
            S_GetPawnDebugString(InLa_Context:GetSelfActor()),
            S_GetPawnDebugString(InLa_Target),
            S_GetPawnDebugString(InLa_Source),
            InLa_OldRadius, InLa_OldHalfHeight,
            InLa_BaseRadius, InLa_BaseHalfHeight,
            InLa_Ability.RadiusValueOrOffset, InLa_Ability.HalfHeightValueOrOffset))
    end, InContext, targetForApply, targetForSource, oldRadius, oldHalfHeight, baseRadius, baseHalfHeight, self)

    local newRadius = baseRadius + self.RadiusValueOrOffset
    local newHalfHeight = baseHalfHeight + self.HalfHeightValueOrOffset

    if oldRadius ~= newRadius or oldHalfHeight ~= newHalfHeight then
        -- 上传未发送的移动帧
        if not self.NotFlushMovement then
            S_FlushServerMovesOnClient(targetForApply.CharacterMovement)
        end

        targetCapsuleForApply:SetCapsuleSize(
            newRadius,
            newHalfHeight,
            true)
    end
end

---@param Context UAbleAbilityContext
function Ability_Task_ChangeCharacterCollisionSize:ApplyWithBlueprint(InContext)
    -- 选取目标
    ---@type ACharacter
    local targetForApply

    if self.ExchangeOwnerAndTarget then
        targetForApply = self.PickFirstTarget(InContext)
    else
        targetForApply = InContext.m_Owner
    end

    if not self.CheckTargetValid(targetForApply) then
        S_DevelopLog(function(InLa_Context)
            _SP.Log("Ability_Task_ChangeCharacterCollisionSize", "Blueprint:not valid target, selfActor",
                S_GetPawnDebugString(InLa_Context:GetSelfActor()))
        end, InContext)
        return
    end
    -- 修改碰撞尺寸
    local targetCapsuleForApply = targetForApply.CapsuleComponent

    if not targetCapsuleForApply then
        S_DevelopLog(function(InLa_Context, InLa_Target)
            _SP.Log("Ability_Task_ChangeCharacterCollisionSize", S_string_format(
                "Blueprint:not valid capsule component, selfActor %s, target %s",
                S_GetPawnDebugString(InLa_Context:GetSelfActor()),
                S_GetPawnDebugString(InLa_Target)))
        end, InContext, targetForApply)
        return
    end

    local oldRadius = 0.0
    local oldHalfHeight = 0.0
    oldRadius, oldHalfHeight = targetCapsuleForApply:GetScaledCapsuleSize(oldRadius, oldHalfHeight)
    local newRadius, newHalfHeight = self.RadiusValueOrOffset, self.HalfHeightValueOrOffset

    S_DevelopLog(function(
        InLa_Context, InLa_Target,
        InLa_OldRadius, InLa_OldHalfHeight,
        InLa_NewRadius, InLa_NewHalfHeight)
        _SP.Log("Ability_Task_ChangeCharacterCollisionSize", S_string_format(
            "Blueprint:selfActor %s, target %s, oldSize(%f,%f) -> newSize(%f,%f)",
            S_GetPawnDebugString(InLa_Context:GetSelfActor()),
            S_GetPawnDebugString(InLa_Target),
            InLa_OldRadius, InLa_OldHalfHeight,
            InLa_NewRadius, InLa_NewHalfHeight))
    end, InContext, targetForApply, oldRadius, oldHalfHeight, newRadius, newHalfHeight)

    if oldRadius ~= newRadius or oldHalfHeight ~= newHalfHeight then
        -- 上传未发送的移动帧
        if not self.NotFlushMovement then
            S_FlushServerMovesOnClient(targetForApply.CharacterMovement)
        end

        targetCapsuleForApply:SetCapsuleSize(
            newRadius,
            newHalfHeight,
            true)
    end
end

---@param Context UAbleAbilityContext
function Ability_Task_ChangeCharacterCollisionSize:ApplyWithContextParams(InContext)
    -- 选取目标
    ---@type ACharacter
    local targetForApply

    if self.ExchangeOwnerAndTarget then
        targetForApply = self.PickFirstTarget(InContext)
    else
        targetForApply = InContext.m_Owner
    end

    if not self.CheckTargetValid(targetForApply) then
        S_DevelopLog(function(InLa_Context)
            _SP.Log("Ability_Task_ChangeCharacterCollisionSize", "ContextParams:not valid target, selfActor",
                S_GetPawnDebugString(InLa_Context:GetSelfActor()))
        end, InContext)
        return
    end
    -- 修改碰撞尺寸
    local targetCapsuleForApply = targetForApply.CapsuleComponent

    if not targetCapsuleForApply then
        S_DevelopLog(function(InLa_Context, InLa_Target)
            _SP.Log("Ability_Task_ChangeCharacterCollisionSize", S_string_format(
                "ContextParams:not valid capsule component, selfActor %s, target %s",
                S_GetPawnDebugString(InLa_Context:GetSelfActor()),
                S_GetPawnDebugString(InLa_Target)))
        end, InContext, targetForApply)
        return
    end

    local newRadius = InContext:GetFloatParameter(self.RadiusParamName)
    local newHalfHeight = InContext:GetFloatParameter(self.HalfHeightParamName)

    if newRadius <= 0 or newHalfHeight <= 0 then
        S_DevelopLog(function(InLa_Context, InLa_Target, InLa_Radius, InLa_HalfHeight)
            _SP.Log("Ability_Task_ChangeCharacterCollisionSize", S_string_format(
                "ContextParams:zero radius %f or half height %f, selfActor %s, target %s",
                InLa_Radius, InLa_HalfHeight,
                S_GetPawnDebugString(InLa_Context:GetSelfActor()),
                S_GetPawnDebugString(InLa_Target)))
        end, InContext, targetForApply, newRadius, newHalfHeight)
        return
    end

    local oldRadius = 0.0
    local oldHalfHeight = 0.0
    oldRadius, oldHalfHeight = targetCapsuleForApply:GetScaledCapsuleSize(oldRadius, oldHalfHeight)

    S_DevelopLog(function(
        InLa_Context, InLa_Target,
        InLa_OldRadius, InLa_OldHalfHeight,
        InLa_NewRadius, InLa_NewHalfHeight)
        _SP.Log("Ability_Task_ChangeCharacterCollisionSize", S_string_format(
            "ContextParams:selfActor %s, target %s, oldSize(%f,%f) -> newSize(%f,%f)",
            S_GetPawnDebugString(InLa_Context:GetSelfActor()),
            S_GetPawnDebugString(InLa_Target),
            InLa_OldRadius, InLa_OldHalfHeight,
            InLa_NewRadius, InLa_NewHalfHeight))
    end, InContext, targetForApply, oldRadius, oldHalfHeight, newRadius, newHalfHeight)

    if oldRadius ~= newRadius or oldHalfHeight ~= newHalfHeight then
        -- 上传未发送的移动帧
        if not self.NotFlushMovement then
            S_FlushServerMovesOnClient(targetForApply.CharacterMovement)
        end

        targetCapsuleForApply:SetCapsuleSize(
            newRadius,
            newHalfHeight,
            true)
    end
end

Ability_Task_ChangeCharacterCollisionSize.ApplyFuncList = {
    Ability_Task_ChangeCharacterCollisionSize.ApplyWithClassDefaultObject,
    Ability_Task_ChangeCharacterCollisionSize.ApplyWithTargetCharacter,
    Ability_Task_ChangeCharacterCollisionSize.ApplyWithBlueprint,
    Ability_Task_ChangeCharacterCollisionSize.ApplyWithContextParams
}

return Ability_Task_ChangeCharacterCollisionSize
