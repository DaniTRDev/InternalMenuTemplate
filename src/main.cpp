#include "Common.hpp"
#include "Pointers/Pointers.hpp"
#include "Hooking/Hook.hpp"
#include "Hooking/Hooking.hpp"

using namespace change_me;



DWORD WINAPI MainThread(LPVOID)
{
	std::filesystem::path BaseDir = std::getenv("appdata");
	BaseDir /= g_CheatName.data();

	FileManager::Get().Initialize(BaseDir);
	Logger::Get().Initialize(g_CheatName, FileManager::Get().GetProjectFile("cout.log"));

	g_GameModuleName = "GFXTest64.exe";

	MH_Initialize();

	try
	{
		LOG(INFO) << "Waiting for game window...";

		while (!FindWindowA("gfx_test", nullptr))
			continue;

		LOG(INFO) << "Initializing ThreadPool";
		ThreadPool::Get().Initialize();
		ThreadPool::Get().SetState(true);

		/*start the components*/
		ModuleManager::Get().Initialize();
		PatternScanner::Get().Initialize();
		Pointers::Get().Initialize();

		while (g_Running)
		{
			/*temporal*/
			if (GetAsyncKeyState(VK_INSERT) & 0x1)
				g_Running = false;

			std::this_thread::sleep_for(1ms);
		}

		LOG(INFO) << "Uninitializing...";

		std::this_thread::sleep_for(100ms); /*make sure everything is stopped*/

		ThreadPool::Get().Uninitialize();
		LOG(INFO) << "Uninitialized ThreadPool!";

		Hooking::Get().Unitialize();

		ModuleManager::Get().Unitialize();

		Renderer::Get().Uninitialize();
		LOG(INFO) << "Uninitialized Renderer!";

		LOG(INFO) << "Dettaching";

		std::this_thread::sleep_for(1000ms); /*make sure everything is uninitialized*/

		Logger::Get().Uninitialize();
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