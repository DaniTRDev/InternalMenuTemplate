#include "Common.hpp"

namespace change_me
{
	BoolWidget::BoolWidget(bool* Bool) : m_Bool(Bool)
	{
		if (!m_Bool)
			LOG(WARNING) << "Initialized a BoolWidget with a NULL value!";
	}

	ImVec2 BoolWidget::GetWidgetSize(float WrapWidth)
	{
		return m_CircleSize;
	}

	void BoolWidget::DrawWidget(ImVec2 Pos, class UIConfiguration Cfg)
	{
		auto Col = (*m_Bool) ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 0, 0, 255);
		ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(Pos.x - m_CircleSize.x, Pos.y + (m_CircleSize.y / 2.f)), 
			m_CircleSize.x, Col);
	}

	void BoolWidget::UpdateInputs(bool& WantCapture, UINT Msg, std::uint32_t Key, LPARAM lParam)
	{
		if (WantCapture && (Msg == WM_KEYDOWN))
		{
			if (Key == VK_SELECT || Key == VK_NUMPAD5)
				*m_Bool ^= true;
		}
	}
}