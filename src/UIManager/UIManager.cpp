#include "Common.hpp"

namespace change_me
{
	void UIManager::Render()
	{
		UpdateDeltaTime();

		auto Sub = m_SubMenus[m_SubMenuIndexes.back()];
		Sub->NewFrame();

		auto Options = Sub->GetOptions();
		auto OptionsCount = Options.size();

		if (OptionsCount)
		{
			HandleInput(Sub.get());
			auto CurrentDrawPos = ImVec2(m_Cfg.m_MenuPos);

			std::size_t OptStartId = 0;
			std::size_t OptEndId = (OptionsCount > m_Cfg.m_MaxVisOptions) ? m_Cfg.m_MaxVisOptions : OptionsCount;

			if (OptionsCount > m_Cfg.m_MaxVisOptions && Sub->GetCurrentOpt() >= m_Cfg.m_MaxVisOptions)
			{
				OptStartId = Sub->GetCurrentOpt() - m_Cfg.m_MaxVisOptions + 1;
				OptEndId = Sub->GetCurrentOpt() + 1;
			}

			auto LastScale = ImGui::GetFont()->Scale;
			ImGui::SetWindowFontScale(m_Cfg.m_OptionTextSize);

			for (auto i = OptStartId; i < OptEndId; i++)
			{
				auto& Opt = Options[i];
				RenderOption(Opt.get(), CurrentDrawPos, i == Sub->GetCurrentOpt());
			}

			RenderScrollBar(ImVec2(m_Cfg.m_MenuPos), CurrentDrawPos, Sub->GetCurrentOpt(), OptionsCount);

			ImGui::SetWindowFontScale(LastScale);
		}

		Sub->EndFrame();

	}
	void UIManager::UpdateDeltaTime()
	{
		m_LastDelta = m_CurrentDelta;
		m_CurrentDelta = float(GetTickCount());

		m_DeltaTime = (m_LastDelta) ? ((m_CurrentDelta - m_LastDelta) / 1000.0f) : 0;
	}

	void UIManager::RenderScroller(const ImVec2& Pos, const ImVec2& Size)
	{
		static float CurrentY = 0.f;
		CurrentY = (CurrentY + (m_DeltaTime + m_Cfg.m_MenuScrollSpeed) * (Pos.y - CurrentY));

		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(Pos.x, CurrentY),
			ImVec2(Pos.x + Size.x, CurrentY + Size.y), m_Cfg.m_MenuScrollColor);

	}
	void UIManager::RenderOption(Option* Opt, ImVec2& Pos, bool CurrentOption)
	{
		auto DrawList = ImGui::GetWindowDrawList();
		auto Widgets = Opt->GetWidgets();

		auto TextSize = Opt->GetOptionTextSize(m_Cfg);
		auto OptSize = Opt->GetOptionSize(m_Cfg);

		/*Draw option box*/
		DrawList->AddRectFilled(Pos, ImVec2(Pos.x + OptSize.x, Pos.y + OptSize.y), m_Cfg.m_OptionBoxColor);

		/*we need to render the scroller before the widgets and the text. otherwise if we put alpha to 255 we won't
		be able to see the text & widgets */
		if (CurrentOption)
			RenderScroller(Pos, OptSize);

		/*draw widgets*/
		ImVec2 WidgetDrawPos = ImVec2(Pos.x + m_Cfg.m_MenuWidth,
			Pos.y + (TextSize.y / 2.f) + m_Cfg.m_OptMargin.y);
		for (auto Widget : Widgets)
		{
			/*draw widgets*/
			WidgetDrawPos.x -= Widget->GetWidgetSize(m_Cfg.m_MenuWidth / 2.f).x + m_Cfg.m_WidgetMargin;
			Widget->DrawWidget(WidgetDrawPos, m_Cfg);
		}

		Pos.x += m_Cfg.m_OptMargin.x; /*add left margin*/
		Pos.y += m_Cfg.m_OptMargin.y; /*add up margin*/

		DrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(Pos.x, Pos.y),
			m_Cfg.m_OptionTextColor, Opt->GetText().data(), 0, m_Cfg.m_MenuWidth / 2.f);

		Pos.x = m_Cfg.m_MenuPos.x; /*reset the x-coord drawing*/
		Pos.y += OptSize.y - m_Cfg.m_OptMargin.y; /*add the option size to the current Y coord*/

	}

	void UIManager::RenderScrollBar(const ImVec2& StartPos, const ImVec2& EndPos, 
		std::size_t SelectedOptionId, std::size_t NumOptions)
	{
		auto TotalYSize = std::roundf(EndPos.y - StartPos.x);
		auto YPerScroll = (TotalYSize) / (NumOptions);

		/*the y-displacement that we are going to add to
		the scroller every time we go up or down in the opt list*/

		/*draw the box of the scroll bar*/
		ImGui::GetWindowDrawList()->AddRectFilled(
			ImVec2(StartPos.x - m_Cfg.m_MenuScrollBarWidth - m_Cfg.m_MenuMargin.x, StartPos.y), 
			ImVec2(EndPos.x + m_Cfg.m_MenuScrollBarWidth - m_Cfg.m_MenuMargin.x, EndPos.y), m_Cfg.m_MenuScrollBarColor);

		static float CurrentY = 0.f;
		CurrentY = (CurrentY + (m_DeltaTime + m_Cfg.m_MenuScrollBarSpeed) *
					( (SelectedOptionId * YPerScroll) - CurrentY ));

		/*draw the scroller*/
		ImGui::GetWindowDrawList()->AddRectFilled(
			ImVec2(StartPos.x - m_Cfg.m_MenuScrollBarWidth - m_Cfg.m_MenuMargin.x, StartPos.y + CurrentY),
			ImVec2(StartPos.x + m_Cfg.m_MenuScrollBarWidth - m_Cfg.m_MenuMargin.x, StartPos.y + CurrentY + YPerScroll),
			m_Cfg.m_MenuScrollColor);

	}

	void UIManager::ScrollForward(SubMenu* Sub)
	{
		Sub->SetSelectedOption((Sub->GetCurrentOpt() == Sub->GetOptions().size() - 1) ? 0 : Sub->GetCurrentOpt() + 1);
	}
	void UIManager::ScrollBackward(SubMenu* Sub)
	{
		Sub->SetSelectedOption((Sub->GetCurrentOpt() == 0) ? Sub->GetOptions().size() - 1 : Sub->GetCurrentOpt() - 1);
	}

	void UIManager::AddSub(std::string_view Name, std::function<void(SubMenu*)> Cb)
	{
		auto Sub = std::make_shared<SubMenu>(Name, Cb);

		if (m_SubMenus.size() == 0)
			m_SubMenuIndexes.push(Sub->GetHash()); /*add our main sub to the queue*/

		m_SubMenus.insert( { Sub->GetHash(), Sub } );
	}

	void UIManager::HandleInput(SubMenu* Sub)
	{
		if (m_Up && m_Cfg.m_UpKeyTimer.OnUpdate())
			ScrollBackward(Sub);

		else if (m_Down && m_Cfg.m_DownKeyTimer.OnUpdate())
			ScrollForward(Sub);
	}
	void UIManager::OnInput(UINT Msg, std::uint32_t Key, LPARAM lParam)
	{
		auto Sub = m_SubMenus[m_SubMenuIndexes.back()];
		auto Opts = Sub->GetOptions();

		if (Msg == WM_KEYDOWN)
		{
			if (Key == VK_UP || Key == VK_NUMPAD8)
				m_Up = true;
			else if (Key == VK_DOWN || Key == VK_NUMPAD2)
				m_Down = true;
		}
		else if (WM_KEYUP)
		{
			if (Key == VK_UP || Key == VK_NUMPAD8)
				m_Up = false;
			else if (Key == VK_DOWN || Key == VK_NUMPAD2)
				m_Down = false;
		}
		else if (Opts.size())
			Opts[Sub->GetCurrentOpt()]->UpdateInput(Msg, Key, lParam);
	}

	void UIManager::Uninitialize()
	{
		for (auto& Sub : m_SubMenus)
			Sub.second->EndFrame();
	
		m_SubMenus.clear();

		for (auto i = 0; i < m_SubMenuIndexes.size(); i++)
			m_SubMenuIndexes.pop();

	}
}