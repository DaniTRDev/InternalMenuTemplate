#pragma once

namespace change_me
{
	class ComponentManager;

	enum class ComponentType : std::uint16_t
	{
		NoNeedsTick,
		NeedsTick,
		NeedsTickOnce
	};

	class ComponentBase
	{
	public:

		friend class ComponentManager;

		ComponentBase(std::string_view Name, ComponentType Type);
		ComponentBase();

		virtual bool Initialize();
		virtual bool Run();
		virtual bool Uninitialize();

		bool IsInitialized();

		std::string_view GetName();
		std::size_t GetIndex();

		ComponentType GetType();

	private:

		void SetIndex(std::size_t Index);
		void SetRunTick(bool State);

	private:

		bool m_Initialized;
		bool m_RunTick;

		std::string_view m_Name;
		std::size_t m_Index;

		ComponentType m_Type;

		
	};
}