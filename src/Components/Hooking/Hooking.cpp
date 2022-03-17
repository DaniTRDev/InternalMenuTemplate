#include "Common.hpp"
#include "Components/Pointers/Pointers.hpp"
#include "Hooking.hpp"
#include "Components/Renderer/Renderer.hpp"

namespace change_me
{
	std::shared_ptr<Hooking> g_Hooking;

	HRESULT WndProcHK(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		if (g_Renderer->WndProc(hWnd, Msg, wParam, lParam))
			return S_OK;

		return g_Pointers->m_Test1->Get()(hWnd, Msg, wParam, lParam);
	}

	HRESULT PresentHK(IDXGISwapChain* ThisPtr, UINT Flags, UINT VSync)
	{
		static std::once_flag Flag;

		std::call_once(Flag, [&]
			{
				g_Renderer->SetD3DPointers(g_Pointers->m_Device->GetPtr(), g_Pointers->m_Context, ThisPtr);

				g_ComponentMgr->AddComponent(g_Renderer);
				g_ComponentMgr->InitializeSingleComponent(g_Renderer);
			});

		g_Renderer->NewFrame();
		g_Renderer->Render();
		g_Renderer->EndFrame();
		return g_Hooking->m_SwapHook->Get<decltype(&PresentHK)>()(ThisPtr, Flags, VSync);
	}

	Hooking::Hooking() : ComponentBase("Hooking", ComponentType::NeedsTick)
		{}

	bool Hooking::Initialize()
	{	
		g_Hooking = g_ComponentMgr->GetComponent<Hooking>("Hooking");
		g_Renderer = std::make_shared<Renderer>(); /*we initialize it here to avoid conflicts*/

		MH_Initialize();
		m_Initialized = true;

		return true;
	}
	bool Hooking::Run()
	{
		if (m_Initialized)
		{
			static std::once_flag Flag;

			if (m_Hooks.size() != 0) /*a quick way of checking the hook queue system is initialized*/
			{
				std::call_once(Flag, [&]
					{
						auto SwapTable = GetSwapChainTable();

						g_Hooking->m_SwapHook = std::make_shared<DHook>("SwapChain", &PresentHK);
						g_Hooking->m_SwapHook->CreateHook(PointerMath(std::uintptr_t(SwapTable[g_SwapChainPresentId])));

						m_Hooks.push_back(m_SwapHook);

						for (auto& Hook : m_Hooks)
							Hook->EnableHook();
					});
			}
		}
		return true;
	}
	bool Hooking::Uninitialize()
	{
		if (m_Initialized)
		{
			for (auto& Hook : m_Hooks)
			{
				Hook->DisableHook();
				Hook->DestroyHook();
			}

			MH_Uninitialize();
		}
		return true;
	}

}