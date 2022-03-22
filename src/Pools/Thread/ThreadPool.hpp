#pragma once

namespace change_me
{
	class ThreadPoolBase
	{
	public:
		friend class ThreadPool;
		using ThreadFunc_t = void(void*);

		ThreadPoolBase(ThreadFunc_t* ThreadFunc, void * Param, std::string_view Name);

		ThreadPoolBase();

		void Run();
		virtual void UnitializeThread() = 0;

		bool IsInitialized();

		HANDLE m_ThreadHandle;
		std::string_view m_Name;

	private:

		ThreadFunc_t* m_ThreadFunc;
		void* m_Param;

	protected:

		bool m_Initialized;

	};

	enum class ThreadEvent
	{
		ThreadEvent_Initialized,
		ThreadEvent_CouldntInitialize,

		ThreadEvent_Uninitialized,
	};

	class ThreadPool
	{
	public:

		static std::shared_ptr<ThreadPool> GetInstance();

		void CreateThread(std::shared_ptr<ThreadPoolBase> Thread);
		void DestroyThread(std::string_view ThreadName);

		void Uninitialize();

		std::shared_ptr<ThreadPoolBase> GetThread(std::string_view ThreadName);
		std::shared_ptr<ThreadPoolBase> GetThread(HANDLE ThreadHandle);

		using ThreadListener_t = std::function<void(ThreadEvent Event)>;
		void AddThreadListener(std::shared_ptr<ThreadPoolBase> Thread, ThreadListener_t Listener);

		void OnThreadEvent(HANDLE ThreadHndl, ThreadEvent Event);

	public:

		HANDLE m_CurrentThread;

		std::vector<std::shared_ptr<ThreadPoolBase>> m_Threads;
		std::vector<std::pair<std::shared_ptr<ThreadPoolBase>, ThreadListener_t>> m_ThreadsListener;

	};  extern std::shared_ptr<ThreadPool> g_ThreadPool;
}

