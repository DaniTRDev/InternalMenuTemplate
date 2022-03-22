#include "Common.hpp"
#include "Threads/Pointers/Pointers.hpp"
#include "Hooking.hpp"
#include "Renderer/Renderer.hpp"

namespace change_me
{
	std::shared_ptr<Hooking> g_Hooking;

	HRESULT WndProcHK(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		if (g_Renderer->WndProc(hWnd, Msg, wParam, lParam))
			return S_OK;

		return g_Pointers->m_Test1.Get()(hWnd, Msg, wParam, lParam);
	}

	HRESULT PresentHK(IDXGISwapChain* ThisPtr, UINT Flags, UINT VSync)
	{
		static std::once_flag Flag;

		std::call_once(Flag, [&]
			{
				g_Renderer->SetD3DPointers(g_Pointers->m_Device.GetPtr(), g_Pointers->m_Context.GetPtr(), ThisPtr);
				g_Renderer->Initialize();
			});

		g_Renderer->NewFrame();
		g_Renderer->Render();
		g_Renderer->EndFrame();
		return g_Pointers->m_SwapChain.Get()(ThisPtr, Flags, VSync);
	}

	Hooking::Hooking() :
		ThreadPoolBase([](void* Param)
			{
				auto Hooks = reinterpret_cast<Hooking*>(Param);

				Hooks->Initialize();
				g_ThreadPool->OnThreadEvent(Hooks->m_ThreadHandle, ThreadEvent::ThreadEvent_Initialized);

				while (Hooks->IsInitialized())
				{
					Hooks->Run();
					Sleep(1);
				}
			
			}, this, "Hooking")
		{}

	bool Hooking::Initialize()
	{	
		MH_Initialize();
		m_Initialized = true;

		return true;
	}
	bool Hooking::Run()
	{
		if (g_PatternScanner->AreCriticalPatternsScanned())
		{
			for (auto& Hook : m_Hooks)
			{
				if (!Hook->m_Created)
					Hook->CreateHook(Hook->m_Target);

				if (!Hook->m_Enabled) /*this will help to dinamicly add hooks after the start up*/
					Hook->EnableHook();
			}
		}

		return true;
	}
	void Hooking::UnitializeThread()
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
	}

}