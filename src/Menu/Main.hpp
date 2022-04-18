#pragma once

namespace change_me
{

	inline void MainMenu(UIManager * UI)
	{
		if (UI->AddTab("MainTab", 0, 0))
		{
			if (ImGui::Begin("asd1"))
			{
				ImGui::Button("asd1");
				if (ImGui::Button("test"))
				{ }
				

			} ImGui::End();
		} UI->PopTab();
		
	}
}