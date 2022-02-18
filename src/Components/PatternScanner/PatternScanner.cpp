#include "Common.hpp"

namespace change_me
{
	Pattern::Pattern(std::string_view Name, std::string_view PatternStr) : m_Name(Name), m_PatternStr(PatternStr)
	{
		if (PatternStr.length() == 0)
		{
			LOG(FATAL) << "The pattern " << Name << "has not a valid signature";
			return;
		}
		TransforPatternStrToBytes();
	}

	std::string_view Pattern::GetName()
	{
		return m_Name;
	}
	std::vector<std::uint8_t>& Pattern::GetPatternBytes()
	{
		return m_PatternBytes;
	}

	void Pattern::TransforPatternStrToBytes()
	{
		for (auto i = 0; i < m_Name.length(); i++)
		{
			if (m_Name[i] == ' ')
				continue;

			if (m_Name[i] == '?')
			{
				if (i < m_Name.length() - 1)
					if (m_Name[i + 1] == '?')
						i++;
				m_PatternBytes.push_back(00); /*it's a wildcard, we don't care of the value*/
			}

			char Characters[2] = { m_Name[i], m_Name[i + 1]};
			auto Converted = strtoul(Characters, nullptr, 16); /*convert the string HEX number(base of 16) to an unsigned
																long number*/

			m_PatternBytes.push_back(std::uint8_t(Converted));
		}
	}

	PatternScanner::PatternScanner() : ComponentBase("PatternScanner", ComponentType::NeedsTickOnce)
	{}
	bool PatternScanner::Initialize()
	{
		auto ModuleMgr = g_ComponentMgr->GetComponent<ModuleManager>("ModuleMgr");
		ModuleMgr->AddModule(std::make_shared<Module>("GoW.exe")); /*add the game module itself*/

		return true;
	}
	bool PatternScanner::Run()
	{
		return true;
	}
	bool PatternScanner::Uninitialize()
	{
		return true;
	}

	void PatternScanner::Add(Pattern* Pattern, std::shared_ptr<Module> Module)
	{
		for (auto& Patt : m_Patterns)
		{
			if (Patt.second->GetName() == Pattern->GetName())
			{
				LOG(WARNING) << "The pattern " << Pattern->GetName() << " has already been added, taking the first one";
				return;
			}
		}

		m_Patterns.push_back(std::make_pair(Pattern, Module));
	}
	void PatternScanner::Scan()
	{
		for (auto& Pat : m_Patterns)
		{
			auto PatternBytes = Pat.first->GetPatternBytes();
			for (auto i = Pat.second->GetBase(); i < Pat.second->GetEnd(); i++)
			{
				auto CurOpCodes = reinterpret_cast<std::uint8_t*>(i);
				for (auto CurOpCodeId = 0; i < PatternBytes.size(); CurOpCodeId++)
				{
					if (PatternBytes[CurOpCodeId] == 0) /*its a wildcard so we don't care of the byte*/
						continue;

					if (PatternBytes[CurOpCodeId] != CurOpCodes[CurOpCodeId])
						break; 

					/*if we reached this place the pattern have been found*/
					auto Ptr = PointerMath(i);

					if (Pat.first->OnAdjustPtr)
						Pat.first->OnAdjustPtr(Ptr);

					if (Pat.first->OnGetPtrsFromPtr)
						Pat.first->OnGetPtrsFromPtr(Ptr);

					if (Pat.first->OnFinalPtr)
						Pat.first->OnFinalPtr(Ptr);

					i = Pat.second->GetEnd(); /*to ensure we don't continue scanning*/
				}
			}
		}
	}
}