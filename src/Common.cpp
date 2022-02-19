#include "Common.hpp"

namespace change_me
{
	std::atomic<bool> g_Running = true;

	HINSTANCE	g_Instance{};
	HANDLE		g_MainThread{};
	DWORD		g_MainThreadId{};

	std::string_view g_GameModuleName;
}