#include "Common.hpp"
#include "Components/Pointers/Pointers.hpp"
#include "Components/Hooking/Hooking.hpp"
#include "Renderer.hpp"
#include "Menu/Main.hpp" /*this one will also include other menuses*/

namespace change_me
{
	std::shared_ptr<Renderer> g_Renderer;

	Renderer::Renderer() : ComponentBase("Renderer", ComponentType::NeedsTick), m_Open(false), m_Device(nullptr),
		m_DeviceContext(nullptr), m_SwapChain(nullptr), m_RenderTarget(nullptr),
		m_BlendState(nullptr), m_BlendColor(), m_OpenTimer(200ms)
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

		auto FontFile = g_FileManager->GetProjectFolder("GUI").GetFile("Fredoka.ttf");

		if (FontFile.DoesFileExist())
			ImGui::GetIO().Fonts->AddFontFromFileTTF(FontFile.GetPath().string().c_str(), 20.f);


		g_AnimationManager = std::make_shared<AnimationManager>();
		m_Notifications = std::make_unique<NotificationManager>();

		m_UIManager = std::make_shared<UIManager>();

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
			m_UIManager.reset();

			m_Notifications->ClearNotifications();

			g_AnimationManager->ClearAnimationQueue();
			g_AnimationManager.reset();

			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();

			m_RenderTarget->Release();
			m_BlendState->Release();

			m_Initialized = false;
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

	std::string Tittle;
	std::string Text;

	void Renderer::Render()
	{
		if (m_Initialized)
		{
			static std::once_flag Flag;
			std::call_once(Flag, [&] 
				{
					MainMenu(m_UIManager.get());
					m_Notifications->PushNotification(g_CheatName.data(), "Menu correctly initialized!");
				});

			if (ImGui::IsKeyDown(ImGuiKey_KeypadMultiply) && m_OpenTimer.OnUpdate())
				m_Open ^= true;

			if (m_Open)
			{
				if (ImGui::Begin("##MainWindow", nullptr, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground
					| ImGuiWindowFlags_NoMove))
				{
					ImGui::SetWindowPos(ImVec2(0, 0));
					ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);

					m_UIManager->Render();

				} ImGui::End();
			}

			if (ImGui::Begin("##NotificationWindow", nullptr,
				ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground
				| ImGuiWindowFlags_NoMove)) /*create an invisible window for the animations*/
			{
				m_Notifications->Run();
				g_AnimationManager->Run();
			
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

		m_UIManager->OnInput(Msg, std::uint32_t(wParam), lParam);/*make our menu update the inputs too*/

		return ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam);;
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
