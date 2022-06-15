#include "Common.hpp"
#include "Pointers/Pointers.hpp"
#include "Hooking.hpp"
#include "Renderer/Renderer.hpp"

namespace change_me
{
	HRESULT WndProcHK(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		if (Renderer::Get().WndProc(hWnd, Msg, wParam, lParam))
			return S_OK;

		return  Pointers::Get().m_Test1.Get()(hWnd, Msg, wParam, lParam);
	}

	struct D3DInfo
	{
		ID3D11Device* d3dDevice;
		IDXGISwapChain* d3dSwapChain;
		ID3D11DeviceContext* d3dContext;
		ID3D11InputLayout* d3dInputLayout;
		ID3D11VertexShader* d3dVertexShader;
		ID3D11PixelShader* d3dPixelShader;
		ID3D11Buffer* d3dVertexBuffer;
		ID3D11Buffer* d3dIndexBuffer;
		ID3D11RenderTargetView* d3dRenderTarget;
		ID3D11Buffer* d3dMatrixConstantBuffer;
		ID3D11Buffer* d3dLineBuffer;
	};

	void FrameTickHK(__int64 ThisPtr, void* a1, const float tick, float fps, wchar_t** gfxInfo, int mousePos[2])
	{
		static bool Init = false;

		if (!Init)
		{
			auto Info = *reinterpret_cast<D3DInfo**>(ThisPtr + 0x28);
			IDXGISwapChain* Swap = Info->d3dSwapChain;

			auto vTablePtr = *(std::uintptr_t***)Swap;

			DHOOK_DECL_RAW(Pointers::Get().m_SwapChain, "SwapChain", PresentHK, vTablePtr[g_SwapChainPresentId]);
			
			Pointers::Get().m_SwapChain.GetHook()->CreateHook();
			Pointers::Get().m_SwapChain.GetHook()->EnableHook();

			Init = true;
		}

		return Pointers::Get().m_FrameTick.Get()(ThisPtr, a1, tick, fps, gfxInfo, mousePos);
	}

	HRESULT PresentHK(IDXGISwapChain* ThisPtr, UINT Flags, UINT VSync)
	{
		static std::once_flag Flag;

		std::call_once(Flag, [&] 
			{
				ID3D11DeviceContext* Context = nullptr;
				ID3D11Device* Device = nullptr;

				ThisPtr->GetDevice(__uuidof(ID3D11Device), (void**)&Device);
				Device->GetImmediateContext(&Context);

				DXGI_SWAP_CHAIN_DESC Desc;
				ThisPtr->GetDesc(&Desc);

				Pointers::Get().m_Hwnd = Desc.OutputWindow;
				Renderer::Get().Initialize(Device, Context, ThisPtr);
			});

		Renderer::Get().NewFrame();
		Renderer::Get().Render();
		Renderer::Get().EndFrame();

		return Pointers::Get().m_SwapChain.Get()(ThisPtr, Flags, VSync);
	}

	void Hooking::DoHooks()
	{/*this is a very critical function so we are going to use a mutex...*/

		std::scoped_lock lock(m_Mutex);
		for (auto Hk : m_Hooks)
		{
			if (!Hk->m_Created)
				Hk->CreateHook();

			if (!Hk->m_Enabled) /*this will help to dynamically add hooks after the start up*/
				Hk->EnableHook();

		}
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