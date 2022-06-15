#pragma once

#include <Windows.h>

#include <chrono>
using namespace std::chrono_literals;

#include <exception>

#include <atomic>
#include <thread>

#include <filesystem>
#include <iostream>
#include <fstream>

#include <string>
#include <string_view>

#include <functional>
#include <mutex>

#include <format>

#pragma comment (lib, "d3d11.lib")
#include <d3d11.h>

#include <MinHook.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>

#include <g3log/g3log.hpp>
#include <g3log/loglevels.hpp>
#include <g3log/logworker.hpp>

#include <nlohmann/json.hpp>

#include "Singleton.hpp"

#include "Timer.hpp"
#include "Memory/PointerMath.hpp"

#include "Pools/Thread/ThreadPool.hpp" /*pools shouldn't be modified*/
#include "Pools/Fiber/FiberPool.hpp"

#include "FileManager/File.hpp"
#include "FileManager/Folder.hpp"
#include "FileManager/FileManager.hpp" /*the file manager should't be modidified at all*/

#include "Logger.hpp" /*logger should not be modified very often*/

#include "Settings/Setting.hpp"
#include "Settings/SettingsListener.hpp"
#include "Settings/SettingsManager.hpp" /*same with settings manager*/

#include "ModuleManager/ModuleManager.hpp" /*same with module manager*/

#include "PatternScanner/Pattern.hpp"
#include "PatternScanner/PatternScanner.hpp" /*same with the pattern scanner*/

#include "Hooking/Hook.hpp" /*this MUST not be modified to avoid conflicts*/

#include "Renderer/AnimationManager/AnimationManager.hpp" /*AnimationManager should not be modified very often*/
#include "Renderer/Notifications/NotificationManager.hpp"
#include "Renderer/UIManager.hpp"
#include "Renderer/Renderer.hpp" /*renderer should not be modified very often, but Menu should*/

#include "Settings/JsonHelper/JsonHelper.hpp"

#define IMVEC4_2_COL32(Vec) IM_COL32(Vec.x, Vec.y, Vec.z, Vec.w)

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
	extern std::uint32_t Joaat(std::string_view Str);
}
