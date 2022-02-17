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

namespace name
{
	using namespace std::chrono_literals;

	inline std::atomic<bool> g_bRunning = true;

	inline HINSTANCE	g_hInstance{};
	inline HANDLE		g_hMainThread{};
	inline DWORD		g_dwMainThreadId{};
}
