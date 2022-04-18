#include "Common.hpp"

namespace change_me
{
	PatternScanner::PatternScanner()
	{}

	bool PatternScanner::Initialize()
	{
		ModuleManager::Get()->AddModule(std::make_shared<Module>(g_GameModuleName)); /*add the game module itself*/
		return true;
	}

	void PatternScanner::OnModule(std::shared_ptr<Module> Mod, OnModuleCB_t* ModCallback, OnFinishedScanCB_t* OnFinishedScanCB)
	{
		if (!Mod || (Mod->GetName().length() == 0))
		{
			LOG(WARNING) << "The module is not valid!";
			return;
		}

		ThreadPool::Get()->PushTask([](PatternScanner* Scanner, std::shared_ptr<Module> Mod,
			OnModuleCB_t* Callback, OnFinishedScanCB_t OnFinishedScanCB)
			{
				while (!Mod->IsModuleLoaded())
					ThreadPool::Get()->Yield();

				Scanner->m_CurrentModule = Mod;
				Callback();

				Scanner->Scan();
				
				ThreadPool::Get()->PushTask(OnFinishedScanCB);/*push another task to not make this one a big charge for
														 the thread*/

				Scanner->m_CurrentModule = nullptr; /*reset the module*/

			}, this, Mod, ModCallback, OnFinishedScanCB);
	}
	void PatternScanner::Add(Pattern Patt, OnScannedPatternCB_t* CB)
	{
		if (!m_CurrentModule)
		{
			LOG(WARNING) << "You must call OnModule with a valid module to add a pattern!";
			return;
		}

		for (auto& PattInfo : m_Patterns)
		{
			if (PattInfo.second.GetName() == Patt.GetName())
			{
				LOG(WARNING) << "The pattern " << Patt.GetName() << " has already been added, taking the first one";
				return;
			}
		}

		m_Patterns.push_back(std::make_pair(CB, Patt));
	}
	
	void PatternScanner::Scan()
	{
		ScanPatternsForModule(m_Patterns);
	}
	void PatternScanner::ScanPatternsForModule(PatternVec_t& Patterns)
	{
		nlohmann::json ScanResult;

		for (auto i = m_CurrentModule->GetBase(); i < m_CurrentModule->GetEnd(); i++)
		{
			auto OpCodes = (std::uint8_t*)i;

			for (auto& Pat : Patterns)
			{
				if (Pat.second.m_Scanned)
					continue;

				auto& PatternBytes = Pat.second.m_PatternBytes;
				for (auto OpCodeId = 0; OpCodeId < PatternBytes.size(); OpCodeId++)
				{
					if (PatternBytes[OpCodeId] == 0) /*wildcard*/
						continue;

					else if (PatternBytes[OpCodeId] != OpCodes[OpCodeId])
						break;

					if (OpCodeId == PatternBytes.size() - 1)
					{
						auto Address = PointerMath(i); /*if we reached this place it means we found the pattern*/

						if (Pat.first)
							Pat.first(Address); /*adjust the address, rip, sub, add, ...*/

						Pat.second.m_Scanned = true;

					}
				}
			}
		}

		for (auto i = 0; i < Patterns.size(); i++) /*do a second iteration to create the json obj*/
		{
			auto& Pat = Patterns[i];

			ScanResult.push_back(
				{
				{ "Correct", Pat.second.m_Scanned ? "true" : "false" },
				{ "PatternName", Pat.second.m_Name }
				});
		}

		LOG(INFO) << "Scanned patterns in module: " << m_CurrentModule->GetName()  << "\n" << ScanResult.dump(6);
		ScanResult.clear();
	}

	bool PatternScanner::ArePatternsScanned()
	{
		return (m_TotalPatterns != 0 && m_Patterns.size() == 0);
	}
}