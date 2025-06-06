---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by ecoliwu.
--- DateTime: 2024/6/12 15:14
---
local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local SPTlogReasonEnue = require("Feature.StarP.Script.System.Report.SPTlogReasonEnue")

local Ability_Task_ModifyAttribute = UE4.Class(nil, "Ability_Task_ModifyAttribute")

local IsValid = UE4.UKismetSystemLibrary.IsValid
local IsAuthoritative = UE4.USPAbilityFunctionLibrary.IsAuthoritative

local function Log(...)
    _SP.Log("SPAbility", "[Ability_Task_ModifyAttribute]", ...)
end

function Ability_Task_ModifyAttribute:OnTaskStartBP(Context)
    if not self.ModifyAtEndOnly then -- 新增选项，让属性在TaskEnd时候才修改
        if self.AttrType > 0 then
            self:ModifyAttributes(Context, self.ModifyType, self.AttrType, self.OpValue, self.OpType)
        end
    end
end

function Ability_Task_ModifyAttribute:OnTaskEndBP(Context)
    if not self.ModifyAtEndOnly then
        if self.bRollBack and self.AttrType > 0 then
            if self.ModifyType == 0 then
                self:ModifyAttributes(Context, self.ModifyType, self.AttrType, -self.OpValue, self.OpType)
            elseif self.ModifyType == 1 then
                -- 读取旧数据进行回滚
                local ScratchPad = self:GetScratchPad(Context)
                if ScratchPad and ScratchPad.OldSetValue then
                    self:ModifyAttributes(Context, self.ModifyType, self.AttrType, ScratchPad.OldSetValue, self.OpType, true)
                end
            end
        end
    else
        if self.AttrType > 0 then
            self:ModifyAttributes(Context, self.ModifyType, self.AttrType, self.OpValue, self.OpType)
        end
    end
end

function Ability_Task_ModifyAttribute:IsSingleFrameBP()
    return not self.bRollBack
end

function Ability_Task_ModifyAttribute:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.ModifyAttribute)
end

function Ability_Task_ModifyAttribute:ModifyAttributes(Context, modifyType, attrType, opValue, opType, bIsReverse)
    local TargetArray = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, TargetArray)
    local bCanRollback = TargetArray:Length() == 1 -- 同时修改多角色属性会导致无法存储，ScratchPad中存储裸指针会导致释放有问题，修改一个单位时才可以进行回滚

    for i = 1, TargetArray:Length() do
        local Target = TargetArray:Get(i)
        if Target == nil or not IsValid(Target) or not IsAuthoritative() then
            Log("Target is nil")
            return
        end

        local attributeComponent = _SP.SPSubSystemUtil.GetAttributeDataByAvatar(Target)
        if attributeComponent == nil then
            Log("Target AttributeComponent is nil, Target Display Name = ", Target:GetName())
            return
        end
        if opType == 2 then
            opValue = opValue * 10 -- 数值接口使用万分比,配置使用千分比
        end
        local tTlogPara = {
            Reason = SPTlogReasonEnue.PlayerAttrChangeReason.AbilityAction,
            Actor = Target
        }
        if modifyType == 0 then
            attributeComponent:DSAddAttributeValue(attrType, opValue, opType, tTlogPara)
        elseif modifyType == 1 then
            if not bIsReverse then
                if self.bRollBack and bCanRollback then
                    local ScratchPad = self:GetScratchPad(Context)
                    if ScratchPad then
                        ScratchPad.OldSetValue = attributeComponent:GetSPAttributeValue(attrType, opType)
                    end
                end
            end

            attributeComponent:DSSetAttributeValue(attrType, opValue, opType, tTlogPara)
        end
    end
end

function Ability_Task_ModifyAttribute:ResetScratchPadBP(ScratchPad)
    if ScratchPad then
        ScratchPad.OldSetValue = nil
    end
end

return Ability_Task_ModifyAttribute