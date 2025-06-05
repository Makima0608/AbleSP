local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")

local Ability_Task_ShowBossSkillProgressBar = UE4.Class(nil, "Ability_Task_ShowBossSkillProgressBar")

---@param Context UAblAbilityContext
function Ability_Task_ShowBossSkillProgressBar:OnTaskStartBP(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    -- 利用ScratchPad存取信息
    ScratchPad.SkillInfo = {}
    ScratchPad.ElapsedTime = 0
    ScratchPad.IsTipsShow = false
    -- 获取BossBar控件
    local view = _SP.Client.SPSceneUIManager:GetBossBarViewAndWidget()
    if view then
        ScratchPad.SkillInfo.SkillName = self.SkillName or ""
        ScratchPad.SkillInfo.PlayForward = self.PlayForward
        ScratchPad.SkillInfo.PlayForwardTime = self.PlayForwardTime
        ScratchPad.SkillInfo.SkillPlayTime = self.SkillPlayTime
        -- ProgresssBar颜色设置
        local colorType = self.UseSpecialColorOnStart and 1 or 0
        view:SetSkillBarColor(colorType, true, Context:GetAbilityId())
        -- 技能开始时显示ProgresssBar
        view:ShowSkillProgress(ScratchPad.SkillInfo, self.BeginVisibility, 0, Context:GetAbilityId())
        if self.ShowTipsBar == true and self.CustomDisplayTime == false then
            -- 如果勾选ShowTipsBar且没勾选自定义时间，则Tips会在开始时显示
            view:ShowSkillTextTip(true, self.TextID, Context:GetAbilityId())
            ScratchPad.IsTipsShow = true
        end
    end
end

---@param Context UAblAbilityContext
---@param DeltaTime DeltaTime
function Ability_Task_ShowBossSkillProgressBar:OnTaskTickBP(Context, DeltaTime)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    if ScratchPad.ElapsedTime ~= nil then
        -- 记录Task当前执行的时间
        ScratchPad.ElapsedTime = ScratchPad.ElapsedTime + DeltaTime
    end
    if self.CustomDisplayTime == true then
        -- 使用自定义显示区间
        local view = _SP.Client.SPSceneUIManager:GetBossBarViewAndWidget()
        if view == nil then return end
        if ScratchPad.ElapsedTime >= self.DisplayStartTime and ScratchPad.ElapsedTime <= self.DisplayEndTime and ScratchPad.IsTipsShow == false then
            -- 在自定义区间内显示
            view:ShowSkillTextTip(true, self.TextID, Context:GetAbilityId())
            ScratchPad.IsTipsShow = true
        elseif (ScratchPad.ElapsedTime < self.DisplayStartTime or ScratchPad.ElapsedTime > self.DisplayEndTime) and ScratchPad.IsTipsShow == true then
            -- 在自定义区间外隐藏
            view:ShowSkillTextTip(false, self.TextID, Context:GetAbilityId())
            ScratchPad.IsTipsShow = false
        else
            return
        end
    end
end

---@param Context UAblAbilityContext
function Ability_Task_ShowBossSkillProgressBar:OnTaskEndBP(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    -- 获取BossBar控件
    local view = _SP.Client.SPSceneUIManager:GetBossBarViewAndWidget()
    if view then
        -- ProgresssBar颜色设置
        local colorType = self.UseSpecialColorOnEnd and 1 or 0
        view:SetSkillBarColor(colorType, false, Context:GetAbilityId())
        -- 结束时隐藏相关控件
        local DelayTime = self.EndVisibility and 0 or self.HideDelayTime
        view:ShowSkillProgress(ScratchPad.SkillInfo, self.EndVisibility, DelayTime, Context:GetAbilityId())
        -- Tips隐藏
        view:ShowSkillTextTip(false, self.TextID, Context:GetAbilityId())
    end
    -- 重置存储的信息
    ScratchPad.ElapsedTime = 0
    ScratchPad.SkillInfo = nil
    ScratchPad.IsTipsShow = false
end

function Ability_Task_ShowBossSkillProgressBar:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.ShowBossSkillProgressBar)
end

return Ability_Task_ShowBossSkillProgressBar