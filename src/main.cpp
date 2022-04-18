#include "Common.hpp"
#include "Threads/Pointers/Pointers.hpp"
#include "Threads/Hooking/Hook.hpp"
#include "Threads/Hooking/Hooking.hpp"

using namespace change_me;

DWORD WINAPI MainThread(LPVOID)
{
	std::filesystem::path BaseDir = std::getenv("appdata");
	BaseDir /= g_CheatName.data();

	FileManager::Get()->Initialize(BaseDir);
	Logger::Get()->Initialize(g_CheatName, FileManager::Get()->GetProjectFile("cout.log"));

	g_GameModuleName = "GoW.exe";

	MH_Initialize();

	try
	{
		LOG(INFO) << "Initializing...";

		LOG(INFO) << "Initializing ThreadPool";
		ThreadPool::Get()->Initialize();
		ThreadPool::Get()->SetState(true);

		/*start the components*/
		ModuleManager::Get()->Initialize();
		PatternScanner::Get()->Initialize();
		Pointers::Get()->Initialize();

		while (g_Running)
		{
			/*temporal*/
			if (GetAsyncKeyState(VK_INSERT) & 0x1)
				g_Running = false;

			std::this_thread::sleep_for(1ms);
		}

		LOG(INFO) << "Uninitializing...";

		std::this_thread::sleep_for(100ms); /*make sure everything is stopped*/

		Hooking::Get()->Unitialize();
		Hooking::Get().reset();

		Pointers::Get().reset();
		PatternScanner::Get().reset();

		ModuleManager::Get()->Unitialize();
		ModuleManager::Get().reset();

		ThreadPool::Get()->Uninitialize();
		ThreadPool::Get().reset();

		LOG(INFO) << "Uninitialized ThreadPool!";

		Renderer::Get()->Uninitialize();
		Renderer::Get().reset();

		LOG(INFO) << "Uninitialized Renderer!";

		FileManager::Get().reset();
		LOG(INFO) << "Uninitialized FileManager!";

		LOG(INFO) << "Dettaching";

		std::this_thread::sleep_for(1000ms); /*make sure everything is uninitialized*/

		Logger::Get()->Uninitialize();
		Logger::Get().reset();

		MH_Uninitialize();
	}
	catch (const std::exception& ex)
	{
		LOG(FATAL) << "Failure in MainThread:\n" << ex.what();
	}

	FreeLibraryAndExitThread(g_Instance, EXIT_SUCCESS); /*this will close the current thread == g_MainThread*/
}


bool WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hInst);

		g_Instance = hInst;
		g_MainThread = CreateThread(nullptr, 0, MainThread, nullptr, 0, &g_MainThreadId);

		break;
	case DLL_PROCESS_DETACH:
		g_Running = false;

		break;
	}

	return true;
}