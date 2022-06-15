#include "Common.hpp"
#include "Pointers.hpp"
#include "../Hooking/Hooking.hpp"

namespace change_me
{
	Pointers::Pointers()
		{}

	bool Pointers::Initialize()
	{
		auto& PS = PatternScanner::Get();

		PS.OnModule(ModuleManager::Get().GetModule(g_GameModuleName),
			[] {
				PatternScanner::Get().Add({ "40 55 56", "WndProc" },
					[](PointerMath& Ptr)
					{
						DHOOK_DECL(Pointers::Get().m_Test1, "WndProc", WndProcHK, Ptr, As<std::uintptr_t>());
					});

				PatternScanner::Get().Add({ "89 05 ? ? ? ? 8B 45 03 8B 8B", "FAIL_TEST" },
					[](PointerMath& Ptr)
					{
						
					});
			}
		);

		PS.OnModule(ModuleManager::Get().GetModule("dx11Render64.dll"),
			[] {

				PatternScanner::Get().Add({ "48 8B C4 48 89 58 10", "FrameTick" },
					[](PointerMath& Ptr)
					{
						DHOOK_DECL(Pointers::Get().m_FrameTick, "FrameTick", FrameTickHK, Ptr, As<std::uintptr_t>());
					});

			}
		);

		PS.Scan([=]
			{
				LOG(INFO) << "Finished scanning patterns!";
				Hooking::Get().DoHooks();
			}
		);

		return true;
	}
}