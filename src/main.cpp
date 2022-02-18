#include "Common.hpp"
#include "FileManager.hpp"

using namespace change_me;

DWORD WINAPI MainThread(LPVOID)
{
	std::filesystem::path baseDir = std::getenv("appdata");
	baseDir /= "InternalModMenu";

	g_FileManager = FileManager::GetInstance(baseDir);

	g_Log = Logger::GetInstance("InternalModMenu", g_FileManager->GetProjectFile("cout.log"));
	g_Log->Initialize();

	try
	{
		LOG(INFO) << "Initializing...";

		/*start with the components*/
		g_ComponentMgr = ComponentManager::GetInstance();
		g_ComponentMgr->AddComponent(std::make_shared<ModuleManager>());
		g_ComponentMgr->AddComponent(std::make_shared<PatternScanner>());

		g_ComponentMgr->InitializeComponents();

		while (g_Running)
		{
			/*temporal*/
			if (GetAsyncKeyState(VK_INSERT) & 0x1)
				g_Running = false;

			std::this_thread::sleep_for(100ms);
		}

		LOG(INFO) << "Uninitializing...";

		g_ComponentMgr->UninitializeComponents();
		
		g_ComponentMgr.reset();
		LOG(INFO) << "Uninitialized ComponentManager!";

		g_FileManager.reset();
		LOG(INFO) << "Uninitialized FileManager!";

		std::this_thread::sleep_for(100ms); /*make sure everything is uninitialized*/

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