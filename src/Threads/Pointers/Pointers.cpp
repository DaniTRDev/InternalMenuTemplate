#include "Common.hpp"
#include "Pointers.hpp"
#include "../Hooking/Hooking.hpp"

namespace change_me
{
	std::shared_ptr<Pointers> g_Pointers;
	Pointers::Pointers() :
		ThreadPoolBase(
			[](void* Param)
			{
				auto Ptrs = reinterpret_cast<Pointers*>(Param);
				Ptrs->Initialize();

				g_ThreadPool->OnThreadEvent(Ptrs->m_ThreadHandle, ThreadEvent::ThreadEvent_Initialized);

				while (Ptrs->m_Initialized)
				{
					Ptrs->Run();
					Sleep(1);
				}

			}, this, "Pointers")
		{}

	bool Pointers::Initialize()
	{
		while (!g_PatternScanner->IsInitialized())
			Sleep(1);

		m_Hwnd = FindWindowA("SonySantaMonica", "God of War");

		g_PatternScanner->OnModule(g_ModuleManager->GetModule(g_GameModuleName), [&] 
		{
				g_PatternScanner->Add({ "40 55 53 56 57 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 10", "Test1" }, 
					[this](PointerMath& Ptr)
					{
						m_Test1 = decltype(m_Test1)("Test1", Ptr.As<std::uintptr_t>(), (void*)WndProcHK);
					});

				g_PatternScanner->Add({ "48 8B 0D ? ? ? ? 8B D8 85 DB", "Device" },
					[this](PointerMath& Ptr)
					{
						m_Device = decltype(m_Device)("Device", *Ptr.ReadRel(3).As<std::uintptr_t*>());

						ID3D11DeviceContext* Ctx;
						(*Ptr.ReadRel(3).As<ID3D11Device**>())->GetImmediateContext(&Ctx);

						m_Context = decltype(m_Context)("Context", std::uintptr_t(Ctx));

					});

				g_PatternScanner->Add({ "2B 45 FB 89 05", "Height" },
					[this](PointerMath& Ptr)
					{
						m_Height = decltype(m_Height)("Height", Ptr.Add(3).ReadRel(2));

					});

				g_PatternScanner->Add({ "89 05 ? ? ? ? 8B 45 03", "Width" },
					[this](PointerMath& Ptr)
					{
						m_Width = decltype(m_Width)("Width", Ptr.ReadRel(2));

					});
		});

		g_PatternScanner->DoScan();
		m_Initialized = true;

		return true;
	}
	bool Pointers::Run()
	{
		if (g_PatternScanner->AreCriticalPatternsScanned())
		{
			static std::once_flag Flag;
			std::call_once(Flag, [&]
				{
					auto SwapTable = GetSwapChainTable();

					m_SwapChain = decltype(m_SwapChain)("SwapChain", std::uintptr_t(SwapTable[g_SwapChainPresentId]),
						(void*)PresentHK);
				});
		}
		return true;
	}
	void Pointers::UnitializeThread()
	{
	}
}