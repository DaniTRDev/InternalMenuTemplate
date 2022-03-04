#include "Common.hpp"

namespace change_me
{
	FiberBase::FiberBase(FiberFunc_t* FiberFunc) : m_FiberFunc(FiberFunc)
	{

	}

	void FiberBase::Run()
	{
		__try
		{
			[this]()
			{
				m_FiberFunc();
			}();
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			[this]()
			{
				LOG(INFO) << "Error at fiber";
			}();
		}
		while (true)
			YieldFiber(0ms);
	}
	void FiberBase::YieldFiber(std::chrono::high_resolution_clock::duration WakeAt)
	{
		m_WakeAt = std::chrono::high_resolution_clock::now() + WakeAt;
		SwitchToFiber(m_MainFiber);
	}
}