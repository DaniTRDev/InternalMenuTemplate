#pragma once

namespace change_me
{

	class Timer
	{
	public:

		Timer(std::chrono::high_resolution_clock::duration Delay);
		Timer();

		void SetDelay(std::chrono::high_resolution_clock::duration Delay);

		bool OnUpdate();
		void UpdateInternalTime();

		float GetDeltaFromDelay();
		float GetDeltaFromTime();

	private:

		std::chrono::high_resolution_clock::duration m_Delay;
		std::chrono::high_resolution_clock::time_point m_InternalTime;

		std::chrono::high_resolution_clock::time_point m_CurrentTime;
	};

}