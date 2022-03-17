#pragma once

namespace change_me
{
	template<typename Number>
	class NumberWidget : public WidgetBase
	{
	public:

		NumberWidget(T* Number);

		ImVec2 GetWidgetSize() override;
		void DrawWidget(ImVec2 Pos) override;

		void OnInput(UINT Msg, std::uint32_t Key, LPARAM lParam) override;

	private:

		Number m_Val;

	};
}

