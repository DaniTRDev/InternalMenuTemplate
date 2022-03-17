#include "Common.hpp"

namespace change_me
{
	void OptionParams::AddDesc(std::string_view Desc)
	{
		m_Desc = Desc;
	}

	Option::Option(std::string_view Text, std::function<void(OptionParams*)> Cb) : m_Text(Text), m_Callback(Cb),
		m_Params(std::make_shared<OptionParams>()), m_CaptureInputs(true)
	{
		m_Callback(m_Params.get());
	}

	const std::vector<std::shared_ptr<WidgetBase>>& Option::GetWidgets()
	{
		return m_Params->m_Widgets;
	}
	const std::shared_ptr<OptionParams>& Option::GetParams()
	{
		return m_Params;
	}

	const std::string_view& Option::GetDesc()
	{
		return m_Params->m_Desc;
	}

	const std::string_view& Option::GetText()
	{
		return m_Text;
	}

	ImVec2 Option::GetOptionSize(const UIConfiguration& Cfg)
	{
		return ImVec2(Cfg.m_MenuWidth + (Cfg.m_OptMargin.x * 2), Cfg.m_OptMargin.y * 2 + GetOptionTextSize(Cfg).y);;
	}
	ImVec2 Option::GetOptionTextSize(const UIConfiguration& Cfg)
	{
		return  ImGui::CalcTextSize(m_Text.data(), 0, false, Cfg.m_MenuWidth / 2.f);
	}

	void Option::UpdateInput(UINT Msg, std::uint32_t Key, LPARAM lParam)
	{
		for (auto& Widget : m_Params->m_Widgets)
			Widget->UpdateInputs(m_CaptureInputs, Msg, Key, lParam);
	}
}