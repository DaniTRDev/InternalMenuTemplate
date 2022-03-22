#pragma once

#include <Windows.h>

#include <chrono>
using namespace std::chrono_literals;

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
#include <misc/cpp/imgui_stdlib.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>

#include <g3log/g3log.hpp>
#include <g3log/loglevels.hpp>
#include <g3log/logworker.hpp>

#include "Timer.hpp"
#include "Memory/PointerMath.hpp"

#include "Pools/Thread/ThreadPool.hpp" /*pools shouldn't be modified*/
#include "Pools/Fiber/FiberPool.hpp"

#include "FileManager/File.hpp"
#include "FileManager/Folder.hpp"
#include "FileManager/FileManager.hpp" /*the file manager should't be modidified at all*/

#include "Logger.hpp" /*logger should not be modified very often*/

#include "Threads/ModuleManager/ModuleManager.hpp" /*same with module manager*/

#include "Threads/PatternScanner/Pattern.hpp"
#include "Threads/PatternScanner/PatternScanner.hpp" /*same with the pattern scanner*/

#include "Threads/Hooking/Hook.hpp" /*this MUST not be modified to avoid conflicts*/

#include "Renderer/AnimationManager/AnimationManager.hpp" /*AnimationManager should not be modified very often*/
#include "Renderer/AnimationManager/Animations/NotificationManager.hpp"

#include "Renderer/Renderer.hpp" /*renderer should not be modified very often, but Menu should*/

namespace change_me
{
	using namespace std::chrono_literals;

	extern std::atomic<bool> g_Running;

	extern HINSTANCE	g_Instance;
	extern HANDLE		g_MainThread;
	extern DWORD		g_MainThreadId;

	extern std::string_view g_CheatName;

	extern std::string_view g_GameModuleName; /*for ex: GoW.exe || GTA5.exe */
	extern std::shared_ptr<FiberPool> g_GameFiberPool; /*only used for games that have some short of VM
													   and you must call functions with a specific context*/

	extern std::uintptr_t** GetSwapChainTable();

	extern std::uint32_t Joaat(std::string_view Str);
}
