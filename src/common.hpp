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

namespace change_me
{
	using namespace std::chrono_literals;

	inline std::atomic<bool> g_Running = true;

	inline HINSTANCE	g_Instance{};
	inline HANDLE		g_MainThread{};
	inline DWORD		g_MainThreadId{};
}
