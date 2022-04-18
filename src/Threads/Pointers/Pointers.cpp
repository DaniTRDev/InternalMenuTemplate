#include "Common.hpp"
#include "Pointers.hpp"
#include "../Hooking/Hooking.hpp"

namespace change_me
{
	Pointers::Pointers()
		{}

	bool Pointers::Initialize()
	{
		m_Hwnd = FindWindowA("SonySantaMonica", "God of War");

		PatternScanner::Get()->OnModule(ModuleManager::Get()->GetModule(Joaat(g_GameModuleName)), []
		{
				PatternScanner::Get()->Add({ "40 55 53 56 57 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 10", "Test1" },
					[](PointerMath& Ptr)
					{
						DHOOK_DECL(Pointers::Get()->m_Test1, "Test1", WndProcHK, Ptr, As<std::uintptr_t>());
					});

				PatternScanner::Get()->Add({ "48 8B 0D ? ? ? ? 8B D8 85 DB", "Device" },
					[](PointerMath& Ptr)
					{
						POINTER_DECL(Pointers::Get()->m_Device, "Device", Ptr, ReadRel(3).DerreferenceAsAddr());

						ID3D11DeviceContext* Ctx = nullptr;
						Pointers::Get()->m_Device.GetPtr()->GetImmediateContext(&Ctx);

						POINTER_DECL_RAW(Pointers::Get()->m_Context, "Context", Ctx);

					});

				PatternScanner::Get()->Add({ "2B 45 FB 89 05", "Height" },
					[](PointerMath& Ptr)
					{
						POINTER_DECL(Pointers::Get()->m_Height, "Height", Ptr, Add(3).ReadRel(2));
					});

				PatternScanner::Get()->Add({ "89 05 ? ? ? ? 8B 45 03", "Width" },
					[](PointerMath& Ptr)
					{
						POINTER_DECL(Pointers::Get()->m_Width, "Width", Ptr, ReadRel(2));
					});

				PatternScanner::Get()->Add({ "89 05 ? ? ? ? 8B 45 03 8B 8B", "FAIL_TEST" },
					[](PointerMath& Ptr)
					{
						
					});
			}, []() /*a callback that will be invoked when every pattern has been scanned*/
			{
				auto SwapTable = GetSwapChainTable();

				DHOOK_DECL_RAW(Pointers::Get()->m_SwapChain, "SwapChain", PresentHK, SwapTable[g_SwapChainPresentId]);

				Hooking::Get()->DoHooks();

			});

		return true;
	}
}