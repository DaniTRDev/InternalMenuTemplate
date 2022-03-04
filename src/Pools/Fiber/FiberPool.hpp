#pragma once

namespace change_me
{
	class FiberBase
	{
	public:
		friend class FiberPool;
		using FiberFunc_t = void();

		FiberBase(FiberFunc_t* FiberFunc);

		void Run();
		virtual void Unitialize() = 0;

		void YieldFiber(std::chrono::high_resolution_clock::duration WakeAt);

	private:

		HANDLE m_MainFiber;

		FiberFunc_t* m_FiberFunc;

		std::chrono::high_resolution_clock::time_point m_WakeAt;
	};

	class FiberPool
	{
	public:

		void CreateFiber(std::shared_ptr<FiberBase> Fiber)
		{
			if (!m_MainFiber)
				m_MainFiber = ConvertThreadToFiber(nullptr); /*convert our current thread to a fiber*/

			auto FiberHandle = ::CreateFiber(0, [](void* LpParam) 
				{		
					auto CastedFiber = static_cast<FiberBase*>(LpParam);
					
					CastedFiber->Run();
					CastedFiber->Unitialize();
				}, Fiber.get());
		}
		void RunTick()
		{
			for (auto& Fiber : m_Fibers)
			{
				Fiber.second->m_MainFiber = GetCurrentFiber();

				if (Fiber.second->m_WakeAt <= std::chrono::high_resolution_clock::now())
					SwitchToFiber(Fiber.first);
			}
		}

	private:

		std::map<HANDLE, std::shared_ptr<FiberBase>> m_Fibers;
		HANDLE m_MainFiber;
	};
}

