#include "Common.hpp"

namespace change_me
{
	std::shared_ptr<PatternScanner> g_PatternScanner;

	Pattern::Pattern(std::string_view PatternStr, std::string_view Name) : m_Name(Name), m_PatternStr(PatternStr), m_Ptr(0)
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
				if (m_PatternStr[i + std::uint64_t(1)] == '?')
					i++;

				m_PatternBytes.push_back(0); /*we don't take care of wildcards*/

				continue;
			}

			char Bytes[] = { m_PatternStr[i], m_PatternStr[i + std::uint64_t(1)] }; /*store the 2bytes in a single char*/

			auto ConvertedBytes = strtoull(Bytes, nullptr, 16);

			m_PatternBytes.push_back(std::uint8_t(ConvertedBytes));

			i++;
		}
	}

	PatternScanner::PatternScanner() : ComponentBase("PatternScanner", ComponentType::NeedsTick)
	{}
	bool PatternScanner::Initialize()
	{
		g_PatternScanner = g_ComponentMgr->GetComponent<PatternScanner>("PatternScanner");
		g_ModuleManager->AddModule(std::make_shared<Module>(g_GameModuleName)); /*add the game module itself*/

		m_Initialized = true;

		return true;
	}
	bool PatternScanner::Run()
	{
		if (m_Initialized)
			Scan();

		return true;
	}
	bool PatternScanner::Uninitialize()
	{
		m_Initialized = false;

		return true;
	}

	void PatternScanner::OnModule(std::shared_ptr<Module> Module, std::function<void()> Callback)
	{
		if (!Module || (Module->GetName().length() == 0))
		{
			LOG(WARNING) << "The module is not valid!";
			return;
		}

		m_CurrentModule = Module;
		Callback();

		m_CurrentModule = nullptr; /*reset the module*/
	}
	void PatternScanner::Add(Pattern* Patt)
	{
		if (!m_CurrentModule)
		{
			LOG(WARNING) << "You must call OnModule with a valid module to add a pattern!";
			return;
		}

		for (auto& PattInfo : m_Patterns[m_CurrentModule].first)
		{
			if (PattInfo->GetName() == Patt->GetName())
			{
				LOG(WARNING) << "The pattern " << Patt->GetName() << " has already been added, taking the first one";
				return;
			}
		}

		m_Patterns[m_CurrentModule].first.push_back(std::shared_ptr<Pattern>(Patt));
		m_Patterns[m_CurrentModule].second = false;
	}
	
	void PatternScanner::Scan()
	{
		for (auto& Pat : m_Patterns)
		{
			if (!Pat.first->IsModuleLoaded())
				continue;
			else if (Pat.second.second)
				continue;

			for (auto& PatInfo : Pat.second.first)
				ScanPattern(PatInfo, Pat.first);

			Pat.second.second = true;
		}
	}
	void PatternScanner::ScanPattern(std::shared_ptr<Pattern> PattInfo, std::shared_ptr<Module> Mod)
	{
		for (std::uintptr_t i = Mod->GetBase(); i < Mod->GetEnd(); i++) /*walk throught each game addr*/
		{
			auto OpCodes = reinterpret_cast<std::uint8_t*>(i);
			auto PatternBytes = PattInfo->GetPatternBytes();

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
						<< AddColorToStream(LogColor::CYAN) << "PatternName: " << PattInfo->GetName() << ","
						<< "\t" << AddColorToStream(LogColor::MAGENTA) << "FoundAddress: " << std::hex << i
						<< ResetStreamColor << " }";

					if (PattInfo->OnAdjustPtr)
						PattInfo->OnAdjustPtr(Ptr);

					if (PattInfo->OnGetPtrsFromPtr)
						PattInfo->OnGetPtrsFromPtr(Ptr);

					if (PattInfo->OnFinalPtr)
						PattInfo->OnFinalPtr(Ptr);

					PattInfo->m_Ptr = Ptr;

					i = Mod->GetEnd() + 1;

					break;
				}
			}

			if (i == Mod->GetEnd())
				LOG(WARNING) << "Could not retrieve the pattern " << AddColorToStream(LogColor::CYAN) 
				<< PattInfo->GetName() << ResetStreamColor;
		}
	}
}