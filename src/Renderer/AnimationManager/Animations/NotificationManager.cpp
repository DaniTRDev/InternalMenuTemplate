#include "Common.hpp"

namespace change_me
{

	NotificationManager::NotificationManager() : m_CurrentNotification(0), m_RenderingNotifications(0)
	{}

	void NotificationManager::PushNotification(std::string Tittle, std::string Text)
	{
		auto NotificationAnim = std::make_shared<AnimationBase>( 
				[this, Tittle, Text](float CurrentDelta) mutable
			{
				ImGui::SetWindowPos(ImVec2(0, 0));
				ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);
				
				auto DrawList = ImGui::GetWindowDrawList();
				auto Font = ImGui::GetFont();

				auto Margins = ImVec2(20, 20); /*need to changes this variables to load them from configuration*/

				auto Alpha = 255 * ((CurrentDelta + 0.4f > 0.9f) ? (1.f) : (CurrentDelta + 0.4f));

				auto DisplaySize = ImGui::GetIO().DisplaySize;
				auto MaxBoxWidth = DisplaySize.x / float(4); /*width*/

				auto TextSize = ImGui::CalcTextSize(Text.c_str(), 0, false, MaxBoxWidth);
				auto TittleTextSize = ImGui::CalcTextSize(Tittle.c_str(), 0, false, MaxBoxWidth);

				if (TextSize.x < MaxBoxWidth)
					TextSize.x = MaxBoxWidth;

				auto BoxPos = ImVec2(DisplaySize.x - MaxBoxWidth - (3 * Margins.x), Margins.y);
				auto BoxSize = ImVec2(MaxBoxWidth + 2 * Margins.x, TittleTextSize.y + TextSize.y + 4 * Margins.y);

				/*draw the box*/
				DrawList->AddRectFilled(BoxPos, ImVec2(BoxPos.x + BoxSize.x, BoxPos.y + BoxSize.y), IM_COL32(63, 64, 66, Alpha));

				/*draw tittle*/
				DrawList->AddText( ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(BoxPos.x + (BoxSize.x / 2.f) - (TittleTextSize.x / 2.f), BoxPos.y + Margins.y),
					IM_COL32(255, 255, 255, Alpha), Tittle.c_str(), 0, MaxBoxWidth);

				/*draw the text itself*/
				DrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(BoxPos.x + Margins.x, BoxPos.y + 2 * Margins.x + TittleTextSize.y),
					IM_COL32(255, 255, 255, Alpha), Text.c_str(), 0, MaxBoxWidth);

				/*draw the borders*/
				/*right*/
				DrawList->AddLine(ImVec2(BoxPos.x + BoxSize.x, BoxPos.y), ImVec2(BoxPos.x + BoxSize.x, BoxPos.y + BoxSize.y),
					IM_COL32(221, 0, 255, Alpha), 2.5f);
				/*left*/
				DrawList->AddLine(ImVec2(BoxPos.x, BoxPos.y), ImVec2(BoxPos.x, BoxPos.y + BoxSize.y), IM_COL32(221, 0, 255, Alpha), 2.f);
				/*up*/
				DrawList->AddLine(ImVec2(BoxPos.x, BoxPos.y), ImVec2(BoxPos.x + BoxSize.x, BoxPos.y), IM_COL32(221, 0, 255, Alpha), 2.f);
				/*down*/
				DrawList->AddLine(ImVec2(BoxPos.x, BoxPos.y + BoxSize.y), ImVec2(BoxPos.x + BoxSize.x, BoxPos.y + BoxSize.y)
					, IM_COL32(221, 0, 255, Alpha), 2.f);

				Font->Scale = 1.f;

				m_CurrentNotification++;

			}, 4s);

		m_Notifications.push_back(NotificationAnim);
		}

	void NotificationManager::ClearNotifications()
	{
		m_Notifications.clear();
	}

	void NotificationManager::Run()
	{
		m_CurrentNotification = 0;
		for(auto& Noti : m_Notifications)
		{ 
			if (m_RenderingNotifications < 3) /*we will only render 3 notifications each time*/
			{
				g_AnimationManager->PushNewAnimation(Noti);
				m_RenderingNotifications++;
			}

			m_CurrentNotification++;
		}
	}
}
