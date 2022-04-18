#include "Common.hpp"

namespace change_me
{

	void ThreadPool::Initialize(std::uint32_t NumThreads)
	{
		for (std::uint32_t i = 0; i < NumThreads; i++)
		{
			m_Threads[i] = std::thread(&ThreadWorker, this);
			LOG(INFO) << "Pushed new thread worker at index: " << i;
		}
		m_Initialized = true; /*we initialize the thread pool after to ensure all the threads were created */

	}
	void ThreadPool::Uninitialize()
	{	
		while (m_TotalTasks != m_Tasks.size()) /*there are tasks still running, we will wait until we have finished them*/
			Yield();

		m_Initialized = false; /*make sure we don't accept more tasks*/

		for (auto i = 0; i < m_Threads.size(); i++)
		{
			m_Threads[i].join();
			LOG(INFO) << "Removed thread worker at index: " << i;
		}

		m_Threads.clear();
	}

	void ThreadPool::ThreadWorker(ThreadPool* Pool)
	{
		while (Pool->m_Initialized)
		{
			std::function<void()> Task;
			if (Pool->PopTask(Task) && Pool->m_State) /*if state == false -> we are on pause mode, we will stop executing
													  tasks until it is set to true*/
			{
				Task();
				Pool->m_TotalTasks--;
			}

			Pool->Yield(1ms); /*if there are not tasks just sleep*/
		}
	}

	void ThreadPool::SetState(bool State)
	{
		m_State = State;
	}
	bool ThreadPool::GetState()
	{
		return m_State;
	}

	inline std::size_t ThreadPool::GetNumThreads()
	{
		return m_Threads.size();
	}
	inline std::size_t ThreadPool::GetNumTasks()
	{
		return m_Tasks.size();
	}

	void ThreadPool::Yield(std::chrono::high_resolution_clock::duration Time)
	{
		if (Time.count() == 0)
			std::this_thread::yield();
		else
		std::this_thread::sleep_for(Time);
	}
}
