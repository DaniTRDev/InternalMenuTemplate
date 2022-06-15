#pragma once

namespace change_me
{
	class SettingsListener
	{
	public:
		
		friend class SettingsManager;

		SettingsListener(std::string_view Name);
		std::string_view GetName();

		virtual void Save(); /*you will need to replace this with the function to save/load every setting*/
		virtual void Load(nlohmann::json& Node);

	private:

		bool m_IsRoot; /*tell if the this node is a root node*/
		nlohmann::json& GetBuffer(); /*return the buffer, this method will be only available for SettingsManager*/

	protected:
		std::string_view m_Name;
		nlohmann::json m_Buffer; /*we are going to save a json obj for the files*/
	};
}

