#pragma once

#undef Yield

namespace change_me
{
	class ThreadPool : public Singleton<ThreadPool>
	{
	public:

		void Initialize(std::uint32_t NumThreads = std::thread::hardware_concurrency());
		void Uninitialize();

		static void ThreadWorker(ThreadPool * Pool);

		void SetState(bool State);
		bool GetState();

		template<typename T>
		inline void PushTask(T Func)
		{
			std::scoped_lock Lock(m_Mutex);

			m_TotalTasks++;
			m_Tasks.push(std::function<void()>(Func));
		}

		template<typename T, typename ...Args>
		inline void PushTask(T Func, Args&&... args)
		{
			PushTask([Func, args...] { Func(args...);  });
		}

		std::size_t GetNumThreads();
		std::size_t GetNumTasks();

		void Yield(std::chrono::high_resolution_clock::duration Time = 0ms);

	private:

		inline bool PopTask(std::function<void()>& Task)
		{
			std::scoped_lock Lock(m_Mutex);

			if (m_Tasks.size())
			{
				Task = std::move(m_Tasks.front());
				m_Tasks.pop();

				return true;
			}

			return false;
		}

	private:

		std::map<std::uint32_t, std::thread> m_Threads;

		std::queue<std::function<void()>> m_Tasks;

		std::recursive_mutex m_Mutex;

		bool m_State{ false }; /*used to tell our thread pool to stop executing tasks, usefull when we inject the mod
					     in the game and we want to wait until a certain game state is reached*/
		bool m_Initialized;

		std::size_t m_TotalTasks;

	};

}

