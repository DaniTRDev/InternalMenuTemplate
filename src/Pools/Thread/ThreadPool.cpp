#include "Common.hpp"

namespace change_me
{
	std::shared_ptr<ThreadPool> g_ThreadPool;

	ThreadPoolBase::ThreadPoolBase(ThreadFunc_t* ThreadFunc, void * Param, std::string_view Name) : 
		m_Name(Name), m_ThreadFunc(ThreadFunc), m_Param(Param), m_ThreadHandle(0)
		{}

	ThreadPoolBase::ThreadPoolBase() : m_Name(""), m_ThreadFunc(nullptr), m_Param(nullptr), m_ThreadHandle(0)
		{}

	void ThreadPoolBase::Run()
	{
		__try
		{
			[this]()
			{
				try
				{
					m_ThreadFunc(m_Param);
				}
				catch (std::exception& exp) /*c/cpp exceptions*/
				{
					LOG(WARNING) << "Cpp error at thread " << AddColorToStream(LogColor::YELLOW) << m_Name << ResetStreamColor << "\n\t Error: " << exp.what();
					UnitializeThread();
					TerminateThread(GetCurrentThread(), S_OK); /*this will terminate the thread, so it won't be executed again*/
				}
			}();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			[this]()
			{
				LOG(WARNING) << "Error at thread " << AddColorToStream(LogColor::YELLOW) << m_Name << ResetStreamColor;
				UnitializeThread();
				TerminateThread(GetCurrentThread(), S_OK); /*this will terminate the thread, so it won't be executed again*/
			}();
		}
	}
	bool ThreadPoolBase::IsInitialized()
	{
		return m_Initialized;
	}

	std::shared_ptr<ThreadPool> ThreadPool::GetInstance()
	{
		static auto Ptr = std::make_shared<ThreadPool>();
		return Ptr;
	}

	void ThreadPool::CreateThread(std::shared_ptr<ThreadPoolBase> Thread)
	{
		if (GetThread(Thread->m_Name))
		{
			LOG(INFO) << "Thread " << AddColorToStream(LogColor::YELLOW)
				<< Thread->m_Name << ResetStreamColor << "already added!";
		}

		LOG(INFO) << "Creating new thread " << AddColorToStream(LogColor::YELLOW)
			<< Thread->m_Name << ResetStreamColor;

		auto Handle = ::CreateThread(nullptr, 0, [](void* LpParam) -> DWORD
			{

				auto CastedThread = static_cast<ThreadPoolBase*>(LpParam);

				CastedThread->m_ThreadHandle = OpenThread(THREAD_ALL_ACCESS, 0, GetCurrentThreadId());
				CastedThread->Run(); /*inside this function there's a while*/

				/*this will be only executed when the thread has finished working AT ALL*/

				return S_OK;

			}, Thread.get(), 0, 0);

		m_Threads.push_back(Thread);
	}
	void ThreadPool::DestroyThread(std::string_view ThreadName)
	{
		for (auto i = 0; i < m_Threads.size(); i++)
		{
			auto Thread = m_Threads[i];
			if (Thread->m_Name == ThreadName)
			{
				Thread->UnitializeThread();
				TerminateThread(Thread->m_ThreadHandle, S_OK);

				m_Threads.erase(m_Threads.begin() + i);
			}
		}
		
	}

	std::shared_ptr<ThreadPoolBase> ThreadPool::GetThread(std::string_view ThreadName)
	{
		for (auto& Thread : m_Threads)
		{
			if (Thread->m_Name == ThreadName)
				return Thread;
		}
		return nullptr;
	}
	std::shared_ptr<ThreadPoolBase> ThreadPool::GetThread(HANDLE ThreadHandle)
	{
		for (auto& Thread : m_Threads)
		{
			if (Thread->m_ThreadHandle == ThreadHandle)
				return Thread;
		}
		return nullptr;
	}

	void ThreadPool::AddThreadListener(std::shared_ptr<ThreadPoolBase> Thread, ThreadListener_t Listener)
	{
		/*we don't check if the thread is valid because it might has not been started yet*/
		m_ThreadsListener.push_back(std::make_pair(Thread, Listener));
	}
	void ThreadPool::OnThreadEvent(HANDLE ThreadHndl, ThreadEvent Event)
	{
		auto Thread = GetThread(ThreadHndl);

		for (auto& ThreadListener : m_ThreadsListener)
		{
			if (ThreadListener.first->m_ThreadHandle == ThreadHndl)
				ThreadListener.second(Event);
		}
	}

	void ThreadPool::Uninitialize()
	{
		for (auto& Thread : m_Threads)
		{
			LOG(INFO) << "Terminating thread " << AddColorToStream(LogColor::YELLOW) << Thread->m_Name << ResetStreamColor;
			
			Thread->UnitializeThread();
			OnThreadEvent(Thread->m_ThreadHandle, ThreadEvent::ThreadEvent_Uninitialized);

			for (auto i = 0; i < m_ThreadsListener.size(); i++)
			{
				if (m_ThreadsListener[i].first->m_ThreadHandle == Thread->m_ThreadHandle)
					m_ThreadsListener.erase(m_ThreadsListener.begin() + 1);
			}


			TerminateThread(Thread->m_ThreadHandle, S_OK);
		}

		m_Threads.clear();
	}

}
