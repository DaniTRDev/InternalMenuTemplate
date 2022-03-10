#pragma once

namespace change_me
{
	class FiberPool;

	class FiberBase
	{
	public:
		friend class FiberPool;
		using FiberFunc_t = void();

		FiberBase(FiberFunc_t* FiberFunc, std::size_t Index = 0, std::string_view Name = "");
		FiberBase();

		void Run();
		void Sleep(std::chrono::high_resolution_clock::duration Time = 0ms);

	private:

		void SetFiberData(std::shared_ptr<FiberPool> ParentFiberPool, void* FiberHandle, std::size_t Index);

	private:
		std::string_view m_Name;
		std::size_t m_Index;

		std::chrono::high_resolution_clock::time_point m_WakeAt;

		void* m_FiberHndl;
		FiberFunc_t* m_FiberFunc;

		std::shared_ptr<FiberPool> m_ParentFiberPool;

	};

	class FiberPool /*we can have multiple fiber pools for multiple threads*/
	{
	public:

		friend class FiberBase;

		FiberPool();

		void CreateFiber(std::shared_ptr<FiberBase> Fiber);
		void DeleteFiber(std::size_t Index);

		void Run();
		void Uninitialize();

		std::shared_ptr<FiberBase> GetCurrent();
		std::shared_ptr<FiberBase> GetFiber(std::size_t Index);

	public:

		HANDLE m_MainFiber; /*just use to put other fibers to sleep*/
		std::shared_ptr<FiberBase> m_CurrentFiber;

		std::map<std::size_t, std::shared_ptr<FiberBase>> m_Fibers;

	};
}