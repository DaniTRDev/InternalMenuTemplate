#include "Common.hpp"

namespace change_me
{
	std::shared_ptr<ThreadPool> g_ThreadPool;

	ThreadPoolBase::ThreadPoolBase(ThreadFunc_t* ThreadFunc, std::string_view Name) : 
		m_Name(Name), m_ThreadFunc(ThreadFunc)
		{}

	ThreadPoolBase::ThreadPoolBase() : m_Name(""), m_ThreadFunc(nullptr)
		{}

	void ThreadPoolBase::Run()
	{
		__try
		{
			[this]()
			{
				m_ThreadFunc();
			}();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			[this]()
			{
				LOG(WARNING) << "Error at thread " << AddColorToStream(LogColor::YELLOW) << m_Name << ResetStreamColor;
				Unitialize();
				TerminateThread(GetCurrentThread(), S_OK); /*this will terminate the thread, so it won't be executed again*/
			}();
		}
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

				g_ThreadPool->m_CurrentThread = GetCurrentThread();

				auto CastedThread = static_cast<ThreadPoolBase*>(LpParam);
				CastedThread->Run(); /*inside this function there's a while*/

				/*this will be only executed when the thread has finished working AT ALL*/

				return S_OK;

			}, Thread.get(), 0, 0);

		m_Threads.insert( { Handle, Thread } );
	}
	void ThreadPool::DestroyThread(std::string_view ThreadName)
	{
		for (auto Thread : m_Threads)
		{
			if (Thread.second->m_Name == ThreadName)
			{
				Thread.second->Unitialize();
				TerminateThread(Thread.first, S_OK);
			}
		}
		
	}

	std::shared_ptr<ThreadPoolBase> ThreadPool::GetThread(std::string_view ThreadName)
	{
		for (auto& Thread : m_Threads)
		{
			if (Thread.second->m_Name == ThreadName)
				return Thread.second;
		}
		return nullptr;
	}

	void ThreadPool::Uninitialize()
	{
		for (auto& Thread : m_Threads)
		{
			Thread.second->Unitialize();
			TerminateThread(Thread.first, S_OK);
		}
	}

}
