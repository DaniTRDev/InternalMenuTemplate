#include "common.hpp"
#include "FileManager.hpp"
#include "Logger.hpp"

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

		LOG(G3LOG_DEBUG) << "Test Debug";
		LOG(WARNING) << "Test Warning";
		//LOG(FATAL) << "Test Fatal";

		while (g_Running)
		{
			/*temporal*/
			if (GetAsyncKeyState(VK_INSERT) & 0x1)
				g_Running = false;

			std::this_thread::sleep_for(100ms);
		}
	}
	catch (const std::exception& ex)
	{
		LOG(FATAL) << "Failure in MainThread:\n" << ex.what();
	}

	g_Log->Uninitialize();
	g_Log.reset();

	g_FileManager.reset();

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