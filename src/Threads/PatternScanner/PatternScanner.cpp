#include "Common.hpp"

namespace change_me
{
	std::shared_ptr<PatternScanner> g_PatternScanner;

	PatternScanner::PatternScanner() :
		ThreadPoolBase(
			[](void* Param)
			{
				auto PatScanner = reinterpret_cast<PatternScanner*>(Param);
				PatScanner->Initialize();

				g_ThreadPool->OnThreadEvent(PatScanner->m_ThreadHandle, ThreadEvent::ThreadEvent_Initialized);

				while (PatScanner->m_Initialized)
				{
					PatScanner->Run();
					Sleep(1);
				}

			}, this, "PatternScanner"), m_AreCriticalPatternsScanned(false), m_DoScan(false)
	{

	}

	bool PatternScanner::Initialize()
	{
		while (!g_ModuleManager->IsInitialized())
			Sleep(1);

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
	void PatternScanner::UnitializeThread()
	{
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
	void PatternScanner::Add(Pattern Patt, OnScannedPatternCB_t CB)
	{
		if (!m_CurrentModule)
		{
			LOG(WARNING) << "You must call OnModule with a valid module to add a pattern!";
			return;
		}

		for (auto& PattInfo : m_Patterns[m_CurrentModule])
		{
			if (PattInfo.second.GetName() == Patt.GetName())
			{
				LOG(WARNING) << "The pattern " << Patt.GetName() << " has already been added, taking the first one";
				return;
			}
		}

		m_Patterns[m_CurrentModule].push_back(std::make_pair(CB, Patt));
	}
	
	void PatternScanner::Scan()
	{
		if (m_DoScan)
		{
			bool PatternScanned = false; /*this will be like a chain, when a pattern couldn't be found it will set
										  m_AreCriticalPatternsScanned to false*/
			for (auto& Pat : m_Patterns)
			{
				if (!Pat.first->IsModuleLoaded())
					Sleep(1); /*wait until the next tick*/

				for (auto& PatInfo : Pat.second)
				{
					if (!PatInfo.second.m_Scanned)
						PatternScanned = ScanPattern(PatInfo, Pat.first);
				}
			}

			if (PatternScanned)
				m_AreCriticalPatternsScanned = true;

			m_DoScan = false;
		}
	}
	bool PatternScanner::ScanPattern(std::pair<OnScannedPatternCB_t,Pattern> PattInfo, std::shared_ptr<Module> Mod)
	{
		for (std::uintptr_t i = Mod->GetBase(); i < Mod->GetEnd(); i++) /*walk throught each game addr*/
		{
			auto OpCodes = reinterpret_cast<std::uint8_t*>(i);
			auto PatternBytes = PattInfo.second.GetPatternBytes();

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
						<< AddColorToStream(LogColor::CYAN) << "PatternName: " << PattInfo.second.GetName() << ","
						<< "\t" << AddColorToStream(LogColor::MAGENTA) << "FoundAddress: " << std::hex << i
						<< ResetStreamColor << " }";

					PattInfo.second.m_Scanned = true;

					if (PattInfo.first)
						PattInfo.first(Ptr);

					return true;
				}
			}
		}

		LOG(WARNING) << "Could not retrieve the pattern " << AddColorToStream(LogColor::CYAN)
			<< PattInfo.second.GetName() << ResetStreamColor;
		
		PattInfo.second.m_Scanned = true;

		return false;
	}

	bool PatternScanner::AreCriticalPatternsScanned()
	{
		return m_AreCriticalPatternsScanned;
	}
	void PatternScanner::DoScan()
	{
		m_DoScan = true;
	}
}