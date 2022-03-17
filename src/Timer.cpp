#include "Common.hpp"

namespace change_me
{
	Timer::Timer(std::chrono::high_resolution_clock::duration Delay) : m_InternalTime(std::chrono::high_resolution_clock::now()),
		m_Delay(Delay)
	{}

	Timer::Timer() : m_InternalTime(std::chrono::high_resolution_clock::now()), m_Delay(0ms)
	{}
	
	void Timer::SetDelay(std::chrono::high_resolution_clock::duration Delay)
	{
		m_Delay = Delay;
	}

	bool Timer::OnUpdate()
	{
		m_CurrentTime = std::chrono::high_resolution_clock::now();
		if ((m_CurrentTime.time_since_epoch() - m_InternalTime.time_since_epoch()).count() >= m_Delay.count())
		{
			m_InternalTime = m_CurrentTime;
			return true;
		}

		return false;
	}
	void Timer::UpdateInternalTime()
	{
		m_InternalTime = std::chrono::high_resolution_clock::now();
	}

	float Timer::GetDeltaFromDelay()
	{
		auto DeltaTime = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>
			(m_CurrentTime.time_since_epoch() - m_InternalTime.time_since_epoch()).count();

		auto CastedTotalDelta =
			std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(m_Delay).count();

		return float(DeltaTime) / float(CastedTotalDelta);
	}
	float Timer::GetDeltaFromTime()
	{
		auto DeltaTime = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>
			(m_CurrentTime.time_since_epoch() - m_InternalTime.time_since_epoch()).count();

		return DeltaTime / 10000.f;
	}
}