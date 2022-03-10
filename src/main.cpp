#include "Common.hpp"
#include "Components/Pointers/Pointers.hpp"
#include "Components/Hooking/Hook.hpp"
#include "Components/Hooking/Hooking.hpp"

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

		/*start with the components*/
		g_ComponentMgr = ComponentManager::GetInstance();
		LOG(INFO) << "Initializing ComponentManager";

		g_ComponentMgr->AddComponent(std::make_shared<ModuleManager>());
		g_ComponentMgr->AddComponent(std::make_shared<PatternScanner>());
		g_ComponentMgr->AddComponent(std::make_shared<Pointers>());
		g_ComponentMgr->AddComponent(std::make_shared<Hooking>());

		g_ThreadPool->CreateThread(std::static_pointer_cast<ThreadPoolBase>(g_ComponentMgr));

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

		g_ComponentMgr.reset();
		LOG(INFO) << "Uninitialized ComponentManager!";

		g_FileManager.reset();
		LOG(INFO) << "Uninitialized FileManager!";

		std::this_thread::sleep_for(100ms); /*make sure everything is uninitialized*/

		LOG(INFO) << "Dettaching";

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