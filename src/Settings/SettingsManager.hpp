#pragma once

namespace change_me
{
	class SettingsManager
	{
	public:

		SettingsManager(std::filesystem::path ConfigFilePath);

		void AddListener(std::shared_ptr<SettingsListener> Listener);

		void Save();
		void Load();

	private:

		std::vector<std::shared_ptr<SettingsListener>> m_Listeners;

		std::filesystem::path m_ConfigFilePath;
	};

	extern std::shared_ptr<SettingsManager> g_OptionsSettings;
	extern std::shared_ptr<SettingsManager> g_UISettings;

}

