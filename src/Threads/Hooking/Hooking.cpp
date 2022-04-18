#include "Common.hpp"
#include "Threads/Pointers/Pointers.hpp"
#include "Hooking.hpp"
#include "Renderer/Renderer.hpp"

namespace change_me
{
	HRESULT WndProcHK(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		if (Renderer::Get()->WndProc(hWnd, Msg, wParam, lParam))
			return S_OK;

		return  Pointers::Get()->m_Test1.Get()(hWnd, Msg, wParam, lParam);
	}

	HRESULT PresentHK(IDXGISwapChain* ThisPtr, UINT Flags, UINT VSync)
	{
		static std::once_flag Flag;

		std::call_once(Flag, [&]
			{
				Renderer::Get()->Initialize(Pointers::Get()->m_Device.GetPtr(), Pointers::Get()->m_Context.GetPtr(), ThisPtr);
			});

		Renderer::Get()->NewFrame();
		Renderer::Get()->Render();
		Renderer::Get()->EndFrame();
		return Pointers::Get()->m_SwapChain.Get()(ThisPtr, Flags, VSync);
	}

	void Hooking::DoHooks()
	{
		ThreadPool::Get()->PushTask([](Hooking* Hooking)
			{
				for (auto& Hook : Hooking->m_Hooks)
				{
					if (!Hook->m_Created)
						Hook->CreateHook(Hook->m_Target);

					if (!Hook->m_Enabled) /*this will help to dynamically add hooks after the start up*/
						Hook->EnableHook();
				}
			}, this);

	}
	void Hooking::Unitialize()
	{
		for (auto& Hook : m_Hooks)
		{
			Hook->DisableHook();
			Hook->DestroyHook();
		}
	}

}