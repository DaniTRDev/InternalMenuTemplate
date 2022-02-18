#pragma once

namespace change_me
{
	class ComponentManager;

	class ComponentBase
	{
	public:

		friend class ComponentManager;

		ComponentBase(std::string_view Name);

		virtual bool Initialize();
		virtual bool Run();
		virtual bool Uninitialize();

		bool IsInitialized();

		std::string_view GetName();
		std::size_t GetIndex();

	private:

		void SetIndex(std::size_t Index); /*we don't want users having acces to this option*/
	
	private:

		bool m_Initialized;

		std::string_view m_Name;
		std::size_t m_Index;
	};
}