#include "Common.hpp"

namespace change_me
{
	PatternScanner::PatternScanner()
	{}

	bool PatternScanner::Initialize()
	{
		ModuleManager::Get().AddModule(std::make_shared<Module>(g_GameModuleName)); /*add the game module itself*/
		ModuleManager::Get().AddModule(std::make_shared<Module>("dx11Render64.dll"));
		
		return true;
	}

	void PatternScanner::OnModule(std::shared_ptr<Module> Mod, OnModuleCB_t ModCallback)
	{
		if (!Mod || (Mod->GetName().length() == 0))
		{
			LOG(WARNING) << "The module is not valid!";
			return;
		}

		m_CurrentModule = Mod;
		ModCallback(); 
	}
	void PatternScanner::Add(Pattern Patt, OnScannedPatternCB_t CB)
	{
		if (!m_CurrentModule)
		{
			LOG(WARNING) << "Error, call PatternScanner::OnModule first!";
			return;
		}

		m_ScanQueue[m_CurrentModule].m_Patterns.push_back(std::make_pair(CB, Patt));
	}
	
	void PatternScanner::Scan(OnFinishedScanCB_t Callback)
	{
		ThreadPool::Get().PushTask([](OnFinishedScanCB_t Cb) 
			{
				for (auto& Scan : PatternScanner::Get().GetScanQueue())
				{
					while (!Scan.second.m_Scanned)
						ThreadPool::Get().Yield();
				}

				Cb();

			}, Callback);

		for (auto& Scan : m_ScanQueue)
		{
			ThreadPool::Get().PushTask([] (std::shared_ptr<Module> Mod, ModuleScan* ModScan)
				{
					while (!Mod->IsModuleLoaded())
						ThreadPool::Get().Yield();

					PatternScanner::Get().ScanPatternsForModule(Mod, ModScan->m_Patterns);

					ModScan->m_Patterns.clear();
					ModScan->m_Scanned = true;

				}, Scan.first, &Scan.second);
		}
		
	}
	void PatternScanner::ScanPatternsForModule(std::shared_ptr<Module> Mod, PatternVec_t& Patterns)
	{
		nlohmann::json ScanResult;

		for (auto i = Mod->GetBase(); i < Mod->GetEnd(); i++)
		{
			auto OpCodes = (std::uint8_t*)i;

			for (auto& Pat : Patterns)
			{
				auto& PatternBytes = Pat.second.m_PatternBytes;

				if (Pat.second.m_Scanned)
					continue;

				else if (PatternBytes.size() > (Mod->GetEnd() - i))
					continue;
				
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

		LOG(INFO) << "Scanned patterns in module: " << Mod->GetName()  << "\n" << ScanResult.dump(6);
		ScanResult.clear();
	}
	
	ScanQueue_t& PatternScanner::GetScanQueue()
	{
		return m_ScanQueue;
	}
}