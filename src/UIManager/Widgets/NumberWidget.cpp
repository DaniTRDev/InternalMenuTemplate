#include "Common.hpp"

namespace change_me
{
	template<typename Number>
	NumberWidget<Number>::NumberWidget(Number* Num, Number MaxVal, Number MinVal, Number Step, std::size_t Precission) : 
		m_Val(Num), m_MaxVal(MaxVal), m_MinVal(MinVal),
		m_Precission(Precission), m_Step(Step)
	{
		if (!m_Val)
			LOGF(WARNING, "Initialized a NumberWidget<%s> with NULL value!", typeid(Num).name());
	
		m_OutStream.precision(Precission);
	}

	template<typename Number>
	ImVec2 NumberWidget<Number>::GetWidgetSize(float WrapWidth)
	{
		return ImGui::CalcTextSize(m_OutStream.str().c_str(), 0, false, WrapWidth);;
	}

	template<typename Number>
	void NumberWidget<Number>::DrawWidget(ImVec2 Pos, class UIConfiguration Cfg)
	{
		ImGui::GetWindowDrawList()->AddText(Pos, Cfg.m_OptionTextColor, m_OutStream.str().c_str());
	}

	template<typename Number>
	void NumberWidget<Number>::UpdateInputs(bool& WantCapture, UINT Msg, std::uint32_t Key, LPARAM lParam)
	{
		if (WantCapture && Msg == WM_KEYDOWN)
		{
			static bool DidValChange = false;

			if (Key == VK_LEFT || Key == VK_NUMPAD4)
			{
				if (*m_Val <= m_MinVal)
					*m_Val = m_MaxVal;
				else
					*m_Val -= m_Step;

				DidValChange = true;
			}

			if (Key == VK_RIGHT || Key == VK_NUMPAD6)
			{
				if (*m_Val >= m_MaxVal)
					*m_Val = m_MinVal;
				else
					*m_Val += m_Step;

				DidValChange = true;
			}

			if (DidValChange)
			{
				DidValChange = false;
				m_OutStream.clear();

				if (*m_Val > m_MinVal)
					m_OutStream << "<";

				m_OutStream << *m_Val;

				if (*m_Val < m_MaxVal)
					m_OutStream << ">";
			}
		}
	}
	

	template class NumberWidget<std::int8_t>;
	template class NumberWidget<std::int32_t>;
	template class NumberWidget<std::int64_t>;

	template class NumberWidget<std::uint8_t>;
	template class NumberWidget<std::uint32_t>;
	template class NumberWidget<std::uint64_t>;

	template class NumberWidget<float>;
}
