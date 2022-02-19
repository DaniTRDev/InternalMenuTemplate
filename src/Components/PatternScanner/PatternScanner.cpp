#include "Common.hpp"

namespace change_me
{
	Pattern::Pattern(std::string_view Name, std::string_view PatternStr) : m_Name(Name), m_PatternStr(PatternStr), m_Ptr(0)
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
		for (auto i = 0; i < m_PatternStr.length(); i++)
		{
			if (m_PatternStr[i] == ' ') /*skip spaces*/
				continue;

			if (m_PatternStr[i] == '?')
			{
				if (m_PatternStr[i + 1] == '?')
					i++;

				m_PatternBytes.push_back(0); /*we don't take care of wildcards*/

				continue;
			}

			char Bytes[] = { m_PatternStr[i], m_PatternStr[i + 1] }; /*store the 2bytes in a single char*/

			auto ConvertedBytes = strtoull(Bytes, nullptr, 16);

			m_PatternBytes.push_back(std::uint8_t(ConvertedBytes));

			i++;
		}
	}

	PatternScanner::PatternScanner() : ComponentBase("PatternScanner", ComponentType::NeedsTickOnce)
	{}
	bool PatternScanner::Initialize()
	{
		auto ModuleMgr = g_ComponentMgr->GetComponent<ModuleManager>("ModuleManager");
		ModuleMgr->AddModule(std::make_shared<Module>(g_GameModuleName)); /*add the game module itself*/

		return true;
	}
	bool PatternScanner::Run()
	{
		Scan();
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
		LOG(INFO) << "Scanning patterns...";
		for (auto& Pat : m_Patterns)
		{
			for (std::uintptr_t i = Pat.second->GetBase(); i < Pat.second->GetEnd(); i++) /*walk throught each game addr*/
			{
				auto OpCodes = reinterpret_cast<std::uint8_t*>(i);
				auto PatternBytes = Pat.first->GetPatternBytes();

				for (auto CurOpId = 0; CurOpId < PatternBytes.size(); CurOpId++)
				{
					auto CurOpCode = OpCodes[CurOpId];

					if (PatternBytes[CurOpId] == 0)
						continue; /*the current op code is a wildcard in our signature, so we don't care about it*/

					else if (PatternBytes[CurOpId] != OpCodes[CurOpId])
						break;

					if (CurOpId == PatternBytes.size() - 1) /*if the loop reached this place, we found out the pattern*/
					{

						auto Ptr = PointerMath(i);

						LOG(INFO) << "{ " 
							<< AddColorToStream(LogColor::CYAN) << "PatternName: " << Pat.first->GetName() << ","
							<< "\t" << AddColorToStream(LogColor::MAGENTA) << "FoundAddress: " << std::hex << i 
							<<  ResetStreamColor << " }";

						if (Pat.first->OnAdjustPtr)
							Pat.first->OnAdjustPtr(Ptr);

						if (Pat.first->OnGetPtrsFromPtr)
							Pat.first->OnGetPtrsFromPtr(Ptr);

						if (Pat.first->OnFinalPtr)
							Pat.first->OnFinalPtr(Ptr);

						i = Pat.second->GetEnd() + 1; /*to ensure we don't continue scanning*/

						break;
					}
				}

				if (i == Pat.second->GetEnd())
					LOG(WARNING) << "Error, could not retrieve pattern " << Pat.first->GetName();
			}
		}
	}
}