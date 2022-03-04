#pragma once

#include <Windows.h>

#include <chrono>

#include <exception>

#include <atomic>
#include <thread>

#include <filesystem>
#include <istream>
#include <fstream>

#include <format>
#include <string>
#include <string_view>

#include <functional>
#include <mutex>

#pragma comment (lib, "d3d11.lib")
#include <d3d11.h>

#include <MinHook.h>

#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>

#include <g3log/g3log.hpp>
#include <g3log/loglevels.hpp>
#include <g3log/logworker.hpp>

#include "Memory/PointerMath.hpp"

#include "Pools/Thread/ThreadPool.hpp" /*pools shouldn't be modified*/
#include "Pools/Fiber/FiberPool.hpp"

#include "FileManager/File.hpp"
#include "FileManager/Folder.hpp"
#include "FileManager/FileManager.hpp"

#include "Logger.hpp" /*logger should not be modified very often*/

#include "ComponentManager/ComponentBase.hpp" /*the component system should not even be modified*/
#include "ComponentManager/ComponentManager.hpp"

#include "Components/ModuleManager/ModuleManager.hpp" /*same with module manager*/

#include "Components/PatternScanner/PatternScanner.hpp" /*same with the pattern scanner*/

#include "Components/Hooking/Hook.hpp"

namespace change_me
{
	using namespace std::chrono_literals;

	extern std::atomic<bool> g_Running;

	extern HINSTANCE	g_Instance;
	extern HANDLE		g_MainThread;
	extern DWORD		g_MainThreadId;

	extern std::string_view g_GameModuleName; /*for ex: GoW.exe || GTA5.exe */

	extern std::uintptr_t** GetSwapChainTable();
}
