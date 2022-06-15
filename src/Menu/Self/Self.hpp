#pragma once

namespace change_me
{
	struct Self : public Singleton<Self>, public SettingsListener
	{
		/*type, DefaultValue*/
		Setting<bool, true> m_GodMode{ "GodMode" };
		Setting<float, 1.f> m_SpeedMultiplier{ "SpeedMultiplier" };

		Self();

		void Save() override;
		void Load(nlohmann::json& Node) override;

		void Render();

	};
}