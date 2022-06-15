#include "Common.hpp"
#include "Pointers/Pointers.hpp"

namespace change_me
{
	std::atomic<bool> g_Running = true;

	HINSTANCE	g_Instance{};
	HANDLE		g_MainThread{};
	DWORD		g_MainThreadId{};

	std::string_view g_CheatName = "InternalMenuTemplate";

	std::string_view g_GameModuleName;
	std::shared_ptr<FiberPool> g_GameFiberPool;

	std::uint32_t Joaat(std::string_view Str)
	{
		std::uint32_t Hash = 0;
		for (auto Char : Str)
		{
			Hash += Char;
			Hash += (Hash << 10);
			Hash ^= (Hash >> 6);
		}
		Hash += (Hash << 3);
		Hash ^= (Hash >> 11);
		Hash += (Hash << 15);

		return Hash;
	}
}