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

#include "Logger.hpp" /*logger should not be modified very often*/

#include "ComponentManager/ComponentBase.hpp" /*the component system should not even be modified*/
#include "ComponentManager/ComponentManager.hpp"

namespace change_me
{
	using namespace std::chrono_literals;

	inline std::atomic<bool> g_Running = true;

	inline HINSTANCE	g_Instance{};
	inline HANDLE		g_MainThread{};
	inline DWORD		g_MainThreadId{};
}
