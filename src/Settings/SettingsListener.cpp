#include "Common.hpp"

namespace change_me
{
	SettingsListener::SettingsListener(std::string_view Name) : m_Name(Name)
	{
		if (!Name.length())
			m_IsRoot = true;
	}

	std::string_view SettingsListener::GetName()
	{
		return m_Name;
	}

	void SettingsListener::Save()
	{
		static_assert("Error, you must override this method!");
	}
	void SettingsListener::Load(nlohmann::json& Node)
	{
		static_assert("Error, you must override this method!");
	}

	nlohmann::json& SettingsListener::GetBuffer()
	{
		return m_Buffer;
	}
}
