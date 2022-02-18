#include "Common.hpp"

namespace change_me
{
	std::shared_ptr<ComponentManager> g_ComponentMgr;

	std::shared_ptr<ComponentManager> ComponentManager::GetInstance()
	{
		static auto Ptr = std::make_shared<ComponentManager>();
		return Ptr;
	}

	void ComponentManager::AddComponent(std::shared_ptr<ComponentBase> Component)
	{
		auto Comp = GetComponent(Component->GetName());
		if (Comp == Component)
		{
			LOG(WARNING) << "The component " << Component->GetName() << " have already been added!";
			return;
		}

		LOG(INFO) << "Adding component " << Component->GetName();
		m_Components.push_back(Component);
	}
	void ComponentManager::RemoveComponent(std::string_view Name)
	{
		auto Comp = GetComponent(Name);
		if (Comp->GetName() == "NULL")
		{
			LOG(WARNING) << "The component " << Name << " hadn't been created!!";
			return;
		}

		auto It = std::find(m_Components.begin(), m_Components.end(), Comp);
		if (It == m_Components.end())
		{
			LOG(WARNING) << "The component " << Name << " hadn't been added!";
			return;
		}

		LOG(WARNING) << "The component " << Name << " have been removed!";
		m_Components.erase(It);
	}
	void ComponentManager::RemoveComponent(std::size_t Index)
	{
		auto Comp = GetComponent(Index);
		if (Comp->GetName() == "NULL")
		{
			LOG(WARNING) << "The component at index " << Index << " hadn't been created!!";
			return;
		}

		auto It = std::find(m_Components.begin(), m_Components.end(), Comp);
		if (It == m_Components.end())
		{
			LOG(WARNING) << "The component at index " << Index << " hadn't been added!";
			return;
		}

		LOG(WARNING) << "The component at index " << Index << " have been removed!";
		m_Components.erase(It);
	}

	std::shared_ptr<ComponentBase> ComponentManager::GetComponent(std::string_view Name)
	{
		for (auto& Comp : m_Components)
		{
			if (Comp->GetName() == Name)
				return Comp;
		}

		LOG(WARNING) << "The component " << Name << " has not been created!";
		return std::make_shared<ComponentBase>("NULL"); /*avoid crashings*/
	}
	std::shared_ptr<ComponentBase> ComponentManager::GetComponent(std::size_t Index)
	{
		if (Index >= m_Components.size())
		{
			LOG(WARNING) << "The component at index" << Index << " has not been created!";
			return std::make_shared<ComponentBase>("NULL"); /*avoid crashings*/
		}
		return m_Components[Index];
	}


	std::shared_ptr<ComponentBase> ComponentManager::operator[](std::string_view Name)
	{
		return GetComponent(Name);
	}
	std::shared_ptr<ComponentBase> ComponentManager::operator[](std::size_t Index)
	{
		return GetComponent(Index);
	}

	void ComponentManager::InitializeComponents()
	{
		LOG(INFO) << "Initialising components";
		for (auto& Comp : m_Components)
		{
			if (!Comp->Initialize())
			{
				LOG(WARNING) << "The component " << Comp->GetName() << " could not been initialised!";
				/*if we couldn't initialise the component, remove it*/
				RemoveComponent(Comp->GetIndex());
			}
			else
				LOG(INFO) << "Initialised " << Comp->GetName();
		}
	}
	void ComponentManager::RunComponents() /*used for tick*/
	{
		for (auto& Comp : m_Components)
		{
			if (!Comp->Run())
			{
				LOG(WARNING) << "Couldn't execute the tick of component " << Comp->GetName();
				/*if we couldn't execute the component, remove it*/
				RemoveComponent(Comp->GetIndex());
			}
		}
	}
	void ComponentManager::UninitializeComponents()
	{
		LOG(INFO) << "Uninitialising components";

		for (auto& Comp : m_Components)
			Comp->Uninitialize();
	}
}
