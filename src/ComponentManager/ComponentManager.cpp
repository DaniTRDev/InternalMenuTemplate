#include "Common.hpp"

namespace change_me
{
	std::shared_ptr<ComponentManager> g_ComponentMgr;

	ComponentManager::ComponentManager() : ThreadPoolBase
	([]() 
		{ 
			g_ComponentMgr->InitializeComponents(); 
			
			while (g_ComponentMgr->IsInitialized())
			{
				g_ComponentMgr->RunComponents();
				Sleep(1);
			}
		
		}, "ComponentManager"), m_Initialized(false), m_CurrentComponent(-1)
		{}

	void ComponentManager::Unitialize()
	{
		m_Initialized = false;

		LOG(INFO) << "Uninitializing components";

		for (auto& Comp : m_Components)
			Comp->Uninitialize();
	}

	std::shared_ptr<ComponentManager> ComponentManager::GetInstance()
	{
		static auto Ptr = std::make_shared<ComponentManager>();
		return Ptr;
	}

	void ComponentManager::AddComponent(std::shared_ptr<ComponentBase> Component)
	{
		if (GetComponent<ComponentBase>(Component->GetName().data()))
		{
			LOG(WARNING) << "The component " << AddColorToStream(LogColor::GREEN) 
				<< Component->GetName() << ResetStreamColor << " have already been added!";
			return;
		}

		LOG(INFO) << "Adding component " << AddColorToStream(LogColor::GREEN) << Component->GetName() << ResetStreamColor;
		m_Components.push_back(Component);
	}
	void ComponentManager::RemoveComponent(std::string_view Name)
	{
		auto Comp = GetComponent<ComponentBase>(Name.data());

		if (!Comp)
		{
			LOG(WARNING) << "The component " << AddColorToStream(LogColor::GREEN) 
				<< Name << ResetStreamColor << " hadn't been added!";
			return;
		}

		auto It = std::find(m_Components.begin(), m_Components.end(), Comp);

		LOG(WARNING) << "The component " << AddColorToStream(LogColor::GREEN)
			<< Name << ResetStreamColor << " have been removed!";
		m_Components.erase(It);
	}
	void ComponentManager::RemoveComponent(std::size_t Index)
	{
		auto Comp = GetComponent<ComponentBase>(Index);

		if (!Comp)
		{
			LOG(WARNING) << "The component at index " << Index << " hadn't been added!";
			return;
		}

		auto It = std::find(m_Components.begin(), m_Components.end(), Comp);

		LOG(WARNING) << "The component " << Comp->GetName() << " have been removed!";
		m_Components.erase(It);
	}

	void ComponentManager::InitializeComponents()
	{
		LOG(INFO) << "Initializing components";
		for (auto& Comp : m_Components)
		{
			if (!Comp->Initialize()) /*make sure to not execute the tick of this component*/
			{
				LOG(WARNING) << "The component " << AddColorToStream(LogColor::GREEN)
					<< Comp->GetName() << ResetStreamColor << " could not been initialised!";
				Comp->m_Type = ComponentType::NoNeedsTick;
			}
			else
				LOG(INFO) << "Initialized " << AddColorToStream(LogColor::GREEN) << Comp->GetName() << ResetStreamColor;
		}

		m_Initialized = true;
	}
	void ComponentManager::InitializeSingleComponent(std::shared_ptr<ComponentBase> Component)
	{
		auto Comp = GetComponent<ComponentBase>(Component->GetName().data());
		if (!Comp)
		{
			LOG(WARNING) << "The component " << AddColorToStream(LogColor::GREEN)
				<< Comp->GetName() << ResetStreamColor << " has NOT been added to the component list!";
			return;
		}
		else if (Component->m_Initialized)
		{
			LOG(WARNING) << "The component " << AddColorToStream(LogColor::GREEN)
				<< Comp->GetName() << ResetStreamColor << " has already been initialized!";
			return;
		}

		if (!Component->Initialize())
		{
			LOG(WARNING) << "The component " << AddColorToStream(LogColor::GREEN)
				<< Comp->GetName() << ResetStreamColor << " could not been initialised!";
			Comp->m_Type = ComponentType::NoNeedsTick;
		}
		else
			LOG(INFO) << "Initialized " << AddColorToStream(LogColor::GREEN) << Comp->GetName() << ResetStreamColor;
	}

	void ComponentManager::RunComponents() /*used for tick*/
	{
		m_Mutex.lock();
		m_CurrentComponent = -1;
		for (auto& Comp : m_Components)
		{
			m_CurrentComponent++;

			if (!Comp->m_Initialized)
				continue;

			if (Comp->m_Type == ComponentType::NoNeedsTick)
				continue;

			if (!Comp->Run())
			{
				LOG(WARNING) << "Couldn't execute the tick of component " << AddColorToStream(LogColor::GREEN)
					<< Comp->GetName() << ResetStreamColor;
				/*if we couldn't execute the component, make sure we don't tick it*/
				Comp->m_Type = ComponentType::NoNeedsTick;
			}
			else if (Comp->GetType() == ComponentType::NeedsTickOnce)
				Comp->m_Type = ComponentType::NoNeedsTick;
		}
		m_Mutex.unlock();
	}
	void ComponentManager::UninitializeComponents()
	{
		m_Initialized = false;
	}

	bool ComponentManager::IsInitialized()
	{
		return m_Initialized;
	}
}
