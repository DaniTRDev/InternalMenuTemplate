#include "Common.hpp"
#include "Pointers/Pointers.hpp"
#include "Hooking/Hooking.hpp"
#include "Renderer/UIManager.hpp"
#include "Renderer.hpp"
#include "Menu/MainMenu.hpp" /*this one will also include other menuses*/

namespace change_me
{
	Renderer::Renderer() : m_Initialized(false), m_Open(false), m_Device(nullptr),
		m_DeviceContext(nullptr), m_SwapChain(nullptr), m_RenderTarget(nullptr),
		m_BlendState(nullptr), m_BlendColor()
	{}

	void Renderer::Initialize(ID3D11Device* Device, ID3D11DeviceContext* DeviceContext, IDXGISwapChain* SwapChain)
	{
		if (!Device)
		{
			LOG(WARNING) << "The Device pointer is not valid!";
			return;
		}
		else if (!DeviceContext)
		{
			LOG(WARNING) << "The DeviceContext pointer is not valid!";
			return;
		}
		else if (!SwapChain)
		{
			LOG(WARNING) << "The SwapChain pointer is not valid!";
			return;
		}

		m_Device = Device;
		m_DeviceContext = DeviceContext;
		m_SwapChain = SwapChain;

		if ((!CreateRenderTarget()) || (!CreateBlendState()))
			return;

		InitializeImGui();

		UIManager::Get().Initialize();

		m_Initialized = true;

		return;
	}
	void Renderer::Uninitialize()
	{
		if (m_RenderTarget && m_BlendState)
		{
			g_OptionsSettings->Save();
			g_UISettings->Save();

			NotificationManager::Get().ClearNotifications();
			UIManager::Get().Uninitialize();
			AnimationManager::Get().ClearAnimationQueue();

			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();

			m_RenderTarget->Release();
			m_BlendState->Release();

			m_Initialized = false;
		}
	}

	void Renderer::InitializeImGui()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		if (!ImGui_ImplWin32_Init(Pointers::Get().m_Hwnd))
		{
			LOG(WARNING) << "Couldn't initialize ImGui::Win32!";
			return;
		}
		LOG(INFO) << "Initialized ImGui::Win32";

		if (!ImGui_ImplDX11_Init(m_Device, m_DeviceContext))
		{
			LOG(WARNING) << "Couldn't initialize ImGui::Dx11!";
			return;
		}
		LOG(INFO) << "Initialized ImGui::DX11";
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
		if (m_Initialized)
		{

			if (ImGui::Begin("##MainMenuWindow", nullptr,
				ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground
				| ImGuiWindowFlags_NoMove))
			{
				ImGui::SetWindowPos(ImVec2(0, 0));
				ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);

				if (m_Open)
				{
					MainMenu::Get().Render();
					UIManager::Get().Render();
				}

				NotificationManager::Get().Run();
				AnimationManager::Get().Run();

			} ImGui::End();
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

		auto Res = ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam);

		if (Msg == WM_KEYDOWN && std::uint32_t(wParam) == VK_MULTIPLY)
			m_Open = !m_Open;

		if (m_Open)
		{
			ImGui::GetIO().MouseDrawCursor = true;
			ImGui::GetIO().WantCaptureKeyboard = true;

			ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		}
		else
		{
			ImGui::GetIO().MouseDrawCursor = false;
			ImGui::GetIO().WantCaptureKeyboard = false;

			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
		}

		return Res;
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

	ID3D11Texture2D* Renderer::Create2DTexture(D3D11_TEXTURE2D_DESC* Desc, D3D11_SUBRESOURCE_DATA* ResourceData)
	{
		ID3D11Texture2D* Texture = nullptr;
		m_Device->CreateTexture2D(Desc, ResourceData, &Texture);

		return Texture;
	}
	ID3D11ShaderResourceView* Renderer::CreateShaderResourceView(ID3D11Texture2D* Texture, D3D11_SHADER_RESOURCE_VIEW_DESC* Desc)
	{
		ID3D11ShaderResourceView* ShaderResourceView = nullptr;
		m_Device->CreateShaderResourceView(Texture, Desc, &ShaderResourceView);

		return ShaderResourceView;
	}

	ID3D11ShaderResourceView* Renderer::CreateShaderResourceViewForImg(void* Pixels, std::uint32_t Width,
		std::uint32_t Height, std::string ImgName)
	{
		D3D11_TEXTURE2D_DESC TexDesc;
		ZeroMemory(&TexDesc, sizeof(TexDesc));
		TexDesc.Width = Width;
		TexDesc.Height = Height;
		TexDesc.MipLevels = 1;
		TexDesc.ArraySize = 1;
		TexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		TexDesc.SampleDesc.Count = 1;
		TexDesc.Usage = D3D11_USAGE_DEFAULT;
		TexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		TexDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA ResourceData;
		ResourceData.pSysMem = Pixels;
		ResourceData.SysMemPitch = TexDesc.Width * 4;
		ResourceData.SysMemSlicePitch = 0;

		auto Texture = Create2DTexture(&TexDesc, &ResourceData);
		if (!Texture)
		{
			LOG(WARNING) << "Could not create image texture: [" << ADD_COLOR_TO_TEXT(LogColor::BLUE, ImgName) << "]";
			return nullptr;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceDesc;
		ZeroMemory(&ShaderResourceDesc, sizeof(ShaderResourceDesc));
		ShaderResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		ShaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		ShaderResourceDesc.Texture2D.MipLevels = TexDesc.MipLevels;
		ShaderResourceDesc.Texture2D.MostDetailedMip = 0;

		auto View = CreateShaderResourceView(Texture, &ShaderResourceDesc);

		if (!View)
		{
			LOG(WARNING) << "Could not create image resource view: [" << ADD_COLOR_TO_TEXT(LogColor::BLUE, ImgName) << "]";
			return nullptr;
		}

		Texture->Release();
		return View;
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

	ID3D11Device* Renderer::GetDevice()
	{
		return m_Device;
	}
	ID3D11DeviceContext* Renderer::GetContext()
	{
		return m_DeviceContext;
	}

	IDXGISwapChain* Renderer::GetSwapChain()
	{
		return m_SwapChain;
	}
}
