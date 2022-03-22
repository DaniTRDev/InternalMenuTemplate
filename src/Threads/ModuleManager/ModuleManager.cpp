#include "Common.hpp"

namespace change_me
{
	std::shared_ptr<ModuleManager> g_ModuleManager;

	Module::Module(std::string_view Name) : m_Initialized(false), m_Name(Name), 
		m_Base(0), m_Size(0)
	{}

	Module::Module() : m_Initialized(false), m_Name(""), m_Base(0), m_Size(0)
		{}

	bool Module::TryGetModule()
	{
		m_Base = std::uintptr_t(GetModuleHandleA(m_Name.data()));

		if (m_Base == 0)
			return false;

		auto DosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(m_Base);
		auto NtHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(m_Base + DosHeader->e_lfanew);

		m_Size = NtHeaders->OptionalHeader.SizeOfImage;

		m_Initialized = true;

		return true;
	}
	bool Module::IsModuleLoaded()
	{
		return m_Initialized;
	}

	std::string_view Module::GetName()
	{
		return m_Name;
	}

	std::size_t Module::GetBase()
	{
		return m_Base;
	}
	std::size_t Module::GetSize()
	{
		return m_Size;
	}
	std::size_t Module::GetEnd()
	{
		return m_Base + m_Size;
	}

	PointerMath Module::GetExport(std::string_view Name)
	{
		return PointerMath(std::uintptr_t(GetProcAddress(HMODULE(m_Base), Name.data())));
	}

	ModuleManager::ModuleManager() : 
		ThreadPoolBase(
			[](void* Param) 
				{
					auto ModuleMgr = reinterpret_cast<ModuleManager*>(Param);
					ModuleMgr->Initialize();

					g_ThreadPool->OnThreadEvent(ModuleMgr->m_ThreadHandle, ThreadEvent::ThreadEvent_Initialized);

					while (ModuleMgr->m_Initialized)
					{
						ModuleMgr->Run();
						Sleep(1);
					}

				}, this, "ModuleManager")
	{

	}

	void ModuleManager::AddModule(std::shared_ptr<Module> Mod)
	{
		if (GetModule(Mod->GetName()))
		{
			LOG(WARNING) << "The module " << Mod->GetName() << " has already been added";
			return;
		}

		m_Modules.push_back(Mod);
	}

	bool ModuleManager::Initialize()
	{
		m_Initialized = true;
		return true; 
	}
	bool ModuleManager::Run()
	{
		if (m_Initialized)
		{
			for (auto& Mod : m_Modules)
			{
				if (!Mod->IsModuleLoaded())
					Mod->TryGetModule();
			}
		}
		return true;
	}
	void ModuleManager::UnitializeThread()
	{
		if (m_Initialized)
		{
			m_Modules.clear();
			m_Initialized = false;
		}
	}

	std::shared_ptr<Module> ModuleManager::GetModule(std::string_view Name)
	{
		for (auto& Mod : m_Modules)
		{
			if (Mod->GetName() == Name)
				return Mod;
		}
		return nullptr;
	}
}

