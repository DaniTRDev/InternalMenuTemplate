#include "Common.hpp"
#include "Pointers.hpp"

namespace change_me
{

	Pointers::Pointers() : ComponentBase("Pointers", ComponentType::NoNeedsTick)
		{}

	bool Pointers::Initialize()
	{
		auto ModuleMgr = g_ComponentMgr->GetComponent<ModuleManager>("ModuleManager");
		auto PatScanner = g_ComponentMgr->GetComponent<PatternScanner>("PatternScanner");

		m_Test1 = new PattermImpl<void*>("Test1", "40 55 53 56 57 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 10");		
		PatScanner->Add(m_Test1, ModuleMgr->GetModule(g_GameModuleName));

		return true;
	}
	bool Pointers::Run()
	{
		return true;
	}
	bool Pointers::Uninitialize()
	{
		return true;
	}
}