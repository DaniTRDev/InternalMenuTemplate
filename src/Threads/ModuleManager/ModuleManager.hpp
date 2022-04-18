#pragma once

namespace change_me
{
	class ModuleManager;

	class Module
	{
	public:

		friend class ModuleManager;

		Module(std::string_view Name);
		Module();

		bool TryGetModule();
		bool IsModuleLoaded();

		std::string_view GetName();

		std::size_t GetBase();
		std::size_t GetSize();
		std::size_t GetEnd();

		PointerMath GetExport(std::string_view Name);

	private:

		bool m_Initialized;

		std::string_view m_Name;

		std::uintptr_t m_Base;
		std::uintptr_t m_Size;
	};

	class ModuleManager : public Singleton<ModuleManager>
	{
	public:

		ModuleManager();

		void AddModule(std::shared_ptr<Module> Mod);

		bool Initialize();
		void Unitialize();
		/*full name (+ extension), ex: d3d11.dll*/

		std::shared_ptr<Module> GetModule(std::uint32_t Id);

	private:

		std::map<std::uint32_t, std::shared_ptr<Module>> m_Modules;

	};

}

