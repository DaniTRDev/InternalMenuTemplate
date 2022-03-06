#include "Common.hpp"
#include "Components/Pointers/Pointers.hpp"

namespace change_me
{
	std::atomic<bool> g_Running = true;

	HINSTANCE	g_Instance{};
	HANDLE		g_MainThread{};
	DWORD		g_MainThreadId{};

	std::string_view g_GameModuleName;

	std::uintptr_t** GetSwapChainTable()
	{
		static std::once_flag Flag; /*make sure to only call this shit once*/

		IDXGISwapChain* swapChain = nullptr;
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* context = nullptr;

		std::uintptr_t** SwapTable;

		std::call_once(Flag, [&] {
			D3D_FEATURE_LEVEL featureLevel;
			const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1 };

			DXGI_RATIONAL refreshRate;
			refreshRate.Numerator = 60;
			refreshRate.Denominator = 1;

			DXGI_MODE_DESC bufferDesc;
			bufferDesc.Width = *g_Pointers->m_Width->GetPtr();
			bufferDesc.Height = *g_Pointers->m_Height->GetPtr();
			bufferDesc.RefreshRate = refreshRate;
			bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

			DXGI_SAMPLE_DESC sampleDesc;
			sampleDesc.Count = 1;
			sampleDesc.Quality = 0;

			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			swapChainDesc.BufferDesc = bufferDesc;
			swapChainDesc.SampleDesc = sampleDesc;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = 1;
			swapChainDesc.OutputWindow = g_Pointers->m_Hwnd;
			swapChainDesc.Windowed = 1;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			auto Res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels, 1,
				D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, &featureLevel, &context);

			if (FAILED(Res))
			{
				LOG(WARNING) << "Could not create a dummy swap chain to retrieve present addr!";
				return false;
			}

			LOG(INFO) << "Created a dummy swapchain at addr: GameModBase+0x" << std::uintptr_t(swapChain);

			SwapTable = *(std::uintptr_t***)swapChain;

			device->Release();
			context->Release();
			swapChain->Release();

			});

		return SwapTable;
	}
}