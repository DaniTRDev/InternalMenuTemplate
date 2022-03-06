#pragma once

namespace change_me
{
	class ThreadPoolBase
	{
	public:
		friend class ThreadPool;
		using ThreadFunc_t = void();

		ThreadPoolBase(ThreadFunc_t* ThreadFunc, std::string_view Name);

		ThreadPoolBase();

		void Run();

		virtual void Unitialize() = 0;

	private:
		std::string_view m_Name;

		ThreadFunc_t* m_ThreadFunc;

	};

	class ThreadPool
	{
	public:

		static std::shared_ptr<ThreadPool> GetInstance();

		void CreateThread(std::shared_ptr<ThreadPoolBase> Thread);
		void DestroyThread(std::string_view ThreadName);

		void Uninitialize();

		std::shared_ptr<ThreadPoolBase> GetThread(std::string_view ThreadName);

	public:

		HANDLE m_CurrentThread;

		std::map<HANDLE, std::shared_ptr<ThreadPoolBase>> m_Threads;

	};  extern std::shared_ptr<ThreadPool> g_ThreadPool;
}

