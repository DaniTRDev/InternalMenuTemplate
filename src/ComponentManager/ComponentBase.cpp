#include "Common.hpp"

namespace change_me
{
	ComponentBase::ComponentBase(std::string_view Name, ComponentType Type) : m_Initialized(false), m_Name(Name), m_Index(0),
		m_Type(Type)
		{}
	ComponentBase::ComponentBase()
		{}

	bool ComponentBase::Initialize()
	{
		static_assert("You must override this function!");
		return false;
	}
	bool ComponentBase::Run()
	{
		static_assert("You must override this function!");
		return false;
	}
	bool ComponentBase::Uninitialize()
	{
		static_assert("You must override this function!");
		return false;
	}

	bool ComponentBase::IsInitialized()
	{
		return m_Initialized;
	}

	std::string_view ComponentBase::GetName()
	{
		return m_Name;
	}
	std::size_t ComponentBase::GetIndex()
	{
		return m_Index;
	}

	ComponentType ComponentBase::GetType()
	{
		return m_Type;
	}

	void ComponentBase::SetIndex(std::size_t Index) /*we don't want users having acces to this option*/
	{
		if (m_Index)
		{
			LOG(WARNING) << "The component " << m_Name << " have already an index (id =" << Index << ")";
			return;
		}
		m_Index = Index;
	}

}