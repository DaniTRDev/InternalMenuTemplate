#include "Common.hpp"
#include "Self.hpp"

namespace change_me
{
	Self::Self() : SettingsListener("Self")
	{
	}

	void Self::Save()
	{
		m_GodMode.Save(m_Buffer);
		m_SpeedMultiplier.Save(m_Buffer);
	}
	void Self::Load(nlohmann::json& Node)
	{
		m_GodMode.Load(Node);
		m_SpeedMultiplier.Load(Node);
	}

	void Self::Render()
	{
		ImGui::Checkbox(m_GodMode.Name(), m_GodMode.GetPtr());
		ImGui::DragFloat(m_SpeedMultiplier.Name(), m_SpeedMultiplier.GetPtr());
	}
}