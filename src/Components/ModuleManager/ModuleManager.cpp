#include "Common.hpp"

namespace change_me
{
	Module::Module(std::string_view Name) : m_Initialized(false), m_Name(Name), 
		m_Base(std::uintptr_t(GetModuleHandleA(m_Name.data()))), m_Size(0)
	{
		if (!m_Base)
		{
			LOG(WARNING) << "Could not get the module handle of " << m_Name << " check spelling!";
			return;
		}

		auto DosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(m_Base);
		auto NtHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(m_Base + DosHeader->e_lfanew);

		m_Size = NtHeaders->OptionalHeader.SizeOfImage;

		m_Initialized = true;
	}
	Module::Module() : m_Initialized(false), m_Name(""), m_Base(0), m_Size(0)
		{}

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
		return Module::m_Size;
	}
	std::size_t Module::GetEnd()
	{
		return m_Base + m_Size;
	}

	PointerMath Module::GetExport(std::string_view Name)
	{
		return PointerMath(std::uintptr_t(GetProcAddress(HMODULE(m_Base), Name.data())));
	}

	ModuleManager::ModuleManager() : ComponentBase("ModuleManager", ComponentType::NoNeedsTick)
	{

	}

	void ModuleManager::AddModule(std::shared_ptr<Module> Mod)
	{
		auto DidModuleExist = GetModule(Mod->GetName());
		if (DidModuleExist->GetBase() != 0)
		{
			LOG(WARNING) << "The module " << Mod->GetName() << " has already been added";
			return;
		}

		m_Modules.push_back(Mod);
	}

	bool ModuleManager::Initialize()
	{
		return true; /*we don't have a code for initialisation*/
	}
	bool ModuleManager::Run()
	{
		/*we don't need a tick function*/
		return true;
	}
	bool ModuleManager::Uninitialize()
	{
		m_Modules.clear();

		return true;
	}

	std::shared_ptr<Module> ModuleManager::GetModule(std::string_view Name)
	{
		for (auto& Mod : m_Modules)
		{
			if (Mod->GetName() == Name)
				return Mod;
		}
		return std::make_shared<Module>();
	}
}

