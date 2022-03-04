#include "Common.hpp"
#include "Components/Pointers/Pointers.hpp"
#include "Components/Hooking/Hooking.hpp"
#include "Renderer.hpp"

namespace change_me
{
	std::shared_ptr<Renderer> g_Renderer;

	Renderer::Renderer() : ComponentBase("Renderer", ComponentType::NeedsTick), m_Open(false), m_Device(nullptr),
		m_DeviceContext(nullptr), m_SwapChain(nullptr), m_RenderTarget(nullptr),
		m_BlendState(nullptr), m_BlendColor()
	{}

	bool Renderer::Initialize()
	{

		while ((!m_Device) || (!m_DeviceContext)) /*this will be called from our component manager thread*/
			Sleep(1); /*this is a check to ensure we passed the pointers throught SetD3DPtrs*/

		if ((!CreateRenderTarget()) || (!CreateBlendState()))
			return false;

		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		if (!ImGui_ImplWin32_Init(g_Pointers->m_Hwnd))
		{
			LOG(WARNING) << "Couldn't initialize ImGui::Win32!";
			return false;
		}
		LOG(INFO) << "Initialized ImGui::Win32";

		if (!ImGui_ImplDX11_Init(m_Device, m_DeviceContext))
		{
			LOG(WARNING) << "Couldn't initialize ImGui::Dx11!";
			return false;
		}
		LOG(INFO) << "Initialized ImGui::DX11";

		m_Initialized = true;

		return true;
	}
	bool Renderer::Run()
	{
		/*maily used to save & get variables from gui, such as colors....*/
		return true;
	}
	bool Renderer::Uninitialize()
	{
		if (m_RenderTarget && m_BlendState)
		{
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();

			m_RenderTarget->Release();
			m_BlendState->Release();
		}

		return true;
	}

	void Renderer::NewFrame()
	{
		if (m_Initialized)
		{
			m_DeviceContext->OMSetBlendState(m_BlendState, m_BlendColor, 0xffffffff);

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
		}
	}
	void Renderer::Render()
	{
		if (m_Initialized && m_Open)
		{
			if (ImGui::Begin("asda"))
			{
				ImGui::Button("asd");
				ImGui::End();
			}
		}
	}
	void Renderer::EndFrame()
	{
		if (m_Initialized)
		{
			ImGui::Render();
			m_DeviceContext->OMSetRenderTargets(1, &m_RenderTarget, nullptr);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			m_DeviceContext->OMSetBlendState(m_BlendState, m_BlendColor, 0xffffffff);
		}
	}

	bool Renderer::WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		if (!m_Initialized)
			return false;

		if (Msg == WM_KEYDOWN)
		{
			if (std::uint32_t(wParam) == VK_MULTIPLY)
				m_Open = !m_Open;
		}

		return ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam);
	}

	bool Renderer::SetD3DPointers(ID3D11Device* Device, ID3D11DeviceContext* DeviceContext, IDXGISwapChain* SwapChain)
	{
		if (!Device)
		{
			LOG(WARNING) << "The Device pointer is not valid!";
			return false;
		}
		else if (!DeviceContext)
		{
			LOG(WARNING) << "The DeviceContext pointer is not valid!";
			return false;
		}
		else if (!SwapChain)
		{
			LOG(WARNING) << "The SwapChain pointer is not valid!";
			return false;
		}

		m_Device = Device;
		m_DeviceContext = DeviceContext;

		m_SwapChain = SwapChain;

		return true;
	}

	bool Renderer::CreateRenderTarget()
	{
		ID3D11Texture2D* BackBuffer = nullptr;

		if (FAILED(m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&BackBuffer))))
		{
			LOG(WARNING) << "Couldn't retrieve the BackBuffer of the SwapChain!";
			return false;
		}

		if (FAILED(m_Device->CreateRenderTargetView(BackBuffer, nullptr, &m_RenderTarget)))
		{
			LOG(WARNING) << "Couldn't create the RenderTargetView!";
			return false;
		}
		BackBuffer->Release();
		return true;
	}
	bool Renderer::CreateBlendState()
	{
		D3D11_BLEND_DESC BlendDesc;
		ZeroMemory(&BlendDesc, sizeof(D3D11_BLEND_DESC));

		/*create blend for alpha fixing!!*/
		BlendDesc.RenderTarget[0].BlendEnable = true;
		BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
		BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		if (FAILED(m_Device->CreateBlendState(&BlendDesc, &m_BlendState)))
		{
			LOG(INFO) << "Could not create the BlendState!";
			return false;
		}

		return true;
	}

	void Renderer::PreResize()
	{
		if (m_Initialized)
		{
			if (m_RenderTarget)
				m_RenderTarget->Release();

			ImGui_ImplDX11_InvalidateDeviceObjects();
		}
	}
	void Renderer::PostResize()
	{
		if (m_Initialized)
		{
			CreateRenderTarget();
			ImGui_ImplDX11_CreateDeviceObjects();
		}
	}
}
