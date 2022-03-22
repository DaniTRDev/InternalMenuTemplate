#include "Common.hpp"
#include "Threads/Pointers/Pointers.hpp"
#include "Threads/Hooking/Hook.hpp"
#include "Threads/Hooking/Hooking.hpp"

using namespace change_me;

DWORD WINAPI MainThread(LPVOID)
{
	std::filesystem::path baseDir = std::getenv("appdata");
	baseDir /= g_CheatName.data();

	g_FileManager = FileManager::GetInstance(baseDir);

	g_Log = Logger::GetInstance(g_CheatName, g_FileManager->GetProjectFile("cout.log"));
	g_Log->Initialize();

	g_GameModuleName = "GoW.exe";

	try
	{
		LOG(INFO) << "Initializing...";

		g_ThreadPool = ThreadPool::GetInstance();
		LOG(INFO) << "Initializing ThreadPool";

		g_ModuleManager = std::make_shared<ModuleManager>();
		g_PatternScanner = std::make_shared<PatternScanner>();
		g_Pointers = std::make_shared<Pointers>();
		g_Hooking = std::make_shared<Hooking>();
		g_Renderer = std::make_shared<Renderer>(); /*we initialize it here to avoid conflicts*/

		/*start the threads*/

		g_ThreadPool->CreateThread(g_ModuleManager);
		g_ThreadPool->CreateThread(g_PatternScanner);
		g_ThreadPool->CreateThread(g_Pointers);
		g_ThreadPool->CreateThread(g_Hooking);

		while (g_Running)
		{
			/*temporal*/
			if (GetAsyncKeyState(VK_INSERT) & 0x1)
				g_Running = false;

			std::this_thread::sleep_for(1ms);
		}

		LOG(INFO) << "Uninitializing...";

		std::this_thread::sleep_for(100ms); /*make sure everything is stopped*/

		g_ThreadPool->Uninitialize();
		g_ThreadPool.reset();

		LOG(INFO) << "Uninitialized ThreadPool!";

		g_Renderer->Uninitialize();
		g_Renderer.reset();

		LOG(INFO) << "Uninitialized Renderer!";

		g_FileManager.reset();
		LOG(INFO) << "Uninitialized FileManager!";

		LOG(INFO) << "Dettaching";

		std::this_thread::sleep_for(1000ms); /*make sure everything is uninitialized*/

		g_Log->Uninitialize();
		g_Log.reset();
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