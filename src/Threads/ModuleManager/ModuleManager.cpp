#include "Common.hpp"

namespace change_me
{
	Module::Module(std::string_view Name) : m_Initialized(false), m_Name(Name), 
		m_Base(0), m_Size(0)
	{}

	Module::Module() : m_Initialized(false), m_Name(""), m_Base(0), m_Size(0)
		{}

	bool Module::TryGetModule()
	{
		auto ModHandle = GetModuleHandleA(m_Name.length() != 0 ? m_Name.data() : nullptr);
		m_Base = std::uintptr_t(ModHandle);

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

	ModuleManager::ModuleManager()
	{

	}

	void ModuleManager::AddModule(std::shared_ptr<Module> Mod)
	{
		auto ModId = Joaat(Mod->GetName());
		if (GetModule(ModId))
		{
			LOG(WARNING) << "The module " << Mod->GetName() << " has already been added";
			return;
		}

		m_Modules.insert({ ModId, Mod }); /*insert the module*/

		ThreadPool::Get()->PushTask([](std::shared_ptr<Module> Mod)
			{
				while (!Mod->IsModuleLoaded()) /*add a task to get the module information*/
				{
					Mod->TryGetModule();
					ThreadPool::Get()->Yield();
				}

				LOG(INFO) << "\n\nCached module:\t{\tName: " << Mod->GetName() << "\tBaseAddress: " << std::hex << Mod->GetBase()
					<< "\tModuleSize:" << Mod->GetSize() << "\t}\n";

			}, Mod);
	}

	bool ModuleManager::Initialize()
	{
		return true; 
	}
	void ModuleManager::Unitialize()
	{
		m_Modules.clear();
	}

	std::shared_ptr<Module> ModuleManager::GetModule(std::uint32_t Id)
	{
		auto It = m_Modules.find(Id);

		if (It != m_Modules.end())
			return m_Modules[Id];

		return nullptr;
	}
}

