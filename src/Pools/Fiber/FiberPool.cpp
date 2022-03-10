#include "Common.hpp"

namespace change_me
{

	FiberBase::FiberBase(FiberFunc_t* ThreadFunc, std::size_t Index, std::string_view Name) :
		m_FiberFunc(ThreadFunc), m_Index(Index), m_Name(Name)
	{}

	FiberBase::FiberBase() : m_Name(""), m_FiberFunc(nullptr), m_FiberHndl(nullptr), m_Index(0)
	{}

	void FiberBase::Run()
	{
		__try
		{
			[this]()
			{
				m_FiberFunc();
			}();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			[this]()
			{
				LOG(WARNING) << "Error at fiber " << AddColorToStream(LogColor::YELLOW) 
					<< m_Index << " " << m_Name << ResetStreamColor;
				
				while (true)
				{
					/*put to sleep*/
				}

			}();
		}
	}
	void FiberBase::Sleep(std::chrono::high_resolution_clock::duration Time)
	{
		m_WakeAt = std::chrono::high_resolution_clock::now() + Time;
		SwitchToFiber(m_ParentFiberPool->m_MainFiber);
	}

	void FiberBase::SetFiberData(std::shared_ptr<FiberPool> ParentFiberPool, void* FiberHandle, std::size_t Index)
	{
		m_ParentFiberPool = ParentFiberPool;
		m_FiberHndl = FiberHandle;
		m_Index = Index;
	}

	FiberPool::FiberPool() : m_MainFiber(0)
	{
		if (!IsThreadAFiber())
			m_MainFiber = ConvertThreadToFiber(nullptr);
		else
			m_MainFiber = GetCurrentFiber();
	}

	void FiberPool::CreateFiber(std::shared_ptr<FiberBase> Fiber)
	{
		if (GetFiber(Fiber->m_Index))
		{
			LOG(INFO) << "Fiber " << AddColorToStream(LogColor::YELLOW)
				<< Fiber->m_Index << " " << Fiber->m_Name << ResetStreamColor << "already added!";
		}

		LOG(INFO) << "Creating new fiber " << AddColorToStream(LogColor::YELLOW)
			<< Fiber->m_Index << " " << Fiber->m_Name << ResetStreamColor;

		auto Handle = ::CreateFiber(0, [](void* LpParam) -> void
			{
				auto CastedThread = static_cast<FiberBase*>(LpParam);
				CastedThread->Run(); /*inside this it will surely be a while*/

			}, Fiber.get());
		Fiber->SetFiberData(std::shared_ptr<FiberPool>(this), Handle, m_Fibers.size());

		m_Fibers.insert( { Fiber->m_Index, Fiber } );
	}
	void FiberPool::DeleteFiber(std::size_t Index)
	{
		auto Fiber = GetFiber(Index);

		if (Fiber)
			::DeleteFiber(Fiber->m_FiberHndl);

	}

	std::shared_ptr<FiberBase> FiberPool::GetCurrent()
	{
		return m_CurrentFiber;
	}
	std::shared_ptr<FiberBase> FiberPool::GetFiber(std::size_t Index)
	{
		for (auto& Thread : m_Fibers)
		{
			if (Thread.first == Index)
				return Thread.second;
		}
		return nullptr;
	}

	void FiberPool::Run()
	{
		for (auto& Fiber : m_Fibers)
		{
			m_CurrentFiber = Fiber.second;
			if (std::chrono::high_resolution_clock::now() >= Fiber.second->m_WakeAt)
				SwitchToFiber(Fiber.second->m_FiberHndl);
		}
	}
	void FiberPool::Uninitialize()
	{
		for (auto& Fiber : m_Fibers)
			::DeleteFiber(Fiber.second->m_FiberHndl);

		m_Fibers.clear();
	}

}