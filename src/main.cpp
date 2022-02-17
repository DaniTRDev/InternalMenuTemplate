#include "common.hpp"
#include "FileManager.hpp"
#include "Logger.hpp"

using namespace change_me;

DWORD WINAPI attach_routine(LPVOID);
bool WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hInst);

		g_hInstance = hInst;
		g_hMainThread = CreateThread(nullptr, 0, attach_routine, nullptr, 0, &g_dwMainThreadId);

		break;
	case DLL_PROCESS_DETACH:
		g_bRunning = false;

		break;
	}

	return true;
}

DWORD WINAPI attach_routine(LPVOID)
{
	while (!FindWindowA("grcWindow", "Grand Theft Auto V"))
		std::this_thread::sleep_for(100ms);

	std::filesystem::path base_dir = std::getenv("appdata");
	base_dir /= "InternalModMenu";
	std::unique_ptr<FileManager> file_manager_inst = std::make_unique<FileManager>(base_dir);

	std::unique_ptr<Logger> logger_inst = std::make_unique<Logger>(
		"InternalModMenu",
		file_manager_inst->GetProjectFile("./cout.log")
	);

	try
	{
		LOG(INFO) << "Initializing...";

		while (g_bRunning)
			std::this_thread::sleep_for(100ms);
	}
	catch (const std::exception& ex)
	{
		LOG(FATAL) << "Failure in attach_routine:\n" << ex.what();
	}

	logger_inst.reset();
	file_manager_inst.reset();

	CloseHandle(g_hMainThread);
	FreeLibraryAndExitThread(g_hInstance, EXIT_SUCCESS);
}