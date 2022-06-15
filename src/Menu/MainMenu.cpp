#include "Common.hpp"
#include "MainMenu.hpp"

namespace change_me
{
	void MainMenu::Render()
	{
		static std::once_flag Flag;
		std::call_once(Flag, [&] 
			{
				NotificationManager::Get().PushNotification("FINISHING", "Loading options settings...");
				
				g_OptionsSettings = std::make_shared<SettingsManager>(FileManager::Get().GetProjectFolder("config").GetFile("config.json").GetPath());
				g_OptionsSettings->AddListener(Self::GetPtr()); /*add each tab here*/
				g_OptionsSettings->Load();
			});

		auto& UI = UIManager::Get();

		if (UI.AddTab("Self"))
		{
			Self::Get().Render();
			UI.PopTab();
		}

	}
}