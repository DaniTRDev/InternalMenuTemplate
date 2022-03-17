#pragma once

namespace change_me
{
	template<typename Number>
	class NumberWidget : public WidgetBase
	{
	public:

		NumberWidget(Number* Num, Number MaxVal, Number MinVal, Number Step = 1, std::size_t Precission = 1);

		ImVec2 GetWidgetSize(float WrapWidth) override;
		void DrawWidget(ImVec2 Pos, class UIConfiguration Cfg) override;

		void UpdateInputs(bool& WantCapture, UINT Msg, std::uint32_t Key, LPARAM lParam) override;

	private:

		Number * m_Val;
		Number m_MaxVal;
		Number m_MinVal;

		std::size_t m_Precission; /*only used for floats*/
		Number m_Step;/*how many units are we going to add or substract everytime we use < or >*/

		std::ostringstream m_OutStream;

	};
}

