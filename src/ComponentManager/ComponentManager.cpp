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
		auto Comp = GetComponent<ComponentBase>(Component->GetName().data());
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
		auto Comp = GetComponent<ComponentBase>(Name.data());

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
		auto Comp = GetComponent<ComponentBase>(Index);

		auto It = std::find(m_Components.begin(), m_Components.end(), Comp);
		if (It == m_Components.end())
		{
			LOG(WARNING) << "The component at index " << Index << " hadn't been added!";
			return;
		}

		LOG(WARNING) << "The component " << Comp->GetName() << " have been removed!";
		m_Components.erase(It);
	}

	void ComponentManager::InitializeComponents()
	{
		LOG(INFO) << "Initializing components";
		for (auto& Comp : m_Components)
		{
			if (!Comp->Initialize())
				LOG(WARNING) << "The component " << Comp->GetName() << " could not been initialised!";
			else
				LOG(INFO) << "Initialized " << Comp->GetName();
		}
	}
	void ComponentManager::RunComponents() /*used for tick*/
	{
		__try
		{
			[this]() /* make a lambda to make the compiler happy, otherwise it throws the object unwinding error*/
			{
				for (auto& Comp : m_Components)
				{
					if (Comp->GetType() == ComponentType::NoNeedsTick)
						continue;

					if (!Comp->Run())
					{
						LOG(WARNING) << "Couldn't execute the tick of component " << Comp->GetName();
						/*if we couldn't execute the component, remove it*/
						RemoveComponent(Comp->GetIndex());
					}
					else if (Comp->GetType() == ComponentType::NeedsTickOnce)
						Comp->SetRunTick(false);
				}
			}();

		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{		
			//LOG(WARNING) << "A component threw an exception!";
		}
	}

	void ComponentManager::UninitializeComponents()
	{
		LOG(INFO) << "Uninitializing components";

		for (auto& Comp : m_Components)
			Comp->Uninitialize();
	}
}
