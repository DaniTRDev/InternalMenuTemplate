#pragma once

namespace nlohmann
{
	inline void to_json(json& j, const ImVec2& p)
	{
		j = json{ {"x", p.x}, {"y", p.y} };
	}
	inline void from_json(const json& j, ImVec2& p)
	{
		j.at("x").get_to(p.x);
		j.at("y").get_to(p.y);
	}

	inline void to_json(json& j, const ImVec4& p)
	{
		j = json
		{
			{ "r", p.x },
			{ "g", p.y },
			{ "b", p.z },
			{ "a", p.w }
		};
	}
	inline void from_json(const json& j, ImVec4& p)
	{
		j.at("r").get_to(p.x);
		j.at("g").get_to(p.y);
		j.at("b").get_to(p.z);
		j.at("a").get_to(p.w);
	}

	inline void to_json(json& j, const change_me::UITabConfig& p)
	{
		j["Name"] = p.m_Name;
		j["DisplayName"] = p.m_DisplayName;
		j["FontName"] = p.m_FontName;
		j["IconName"] = p.m_IconName;
	}
	inline void from_json(const json& j, change_me::UITabConfig& p)
	{
		p.m_Name = j["Name"];
		p.m_DisplayName = j["DisplayName"];
		p.m_FontName = j["FontName"];
		p.m_IconName = j["IconName"];
	}

}