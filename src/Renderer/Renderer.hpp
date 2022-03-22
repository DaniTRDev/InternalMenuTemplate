#pragma once

namespace change_me
{
	class Renderer
	{
	public:

		Renderer();
		
		void Initialize();
		void Uninitialize();

	public:

		void NewFrame();
		void Render();
		void EndFrame();

		bool WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	public:

		bool SetD3DPointers(ID3D11Device * Device, ID3D11DeviceContext* DeviceContext, IDXGISwapChain * SwapChain);

		bool CreateRenderTarget();
		bool CreateBlendState();

		void PreResize();
		void PostResize();

	private:

		Timer m_OpenTimer;
		bool m_Open;

		std::unique_ptr<NotificationManager> m_Notifications;

		bool m_Initialized;

	private:

		ID3D11Device* m_Device; /*we save a local copy to make our live easier while coding*/
		ID3D11DeviceContext* m_DeviceContext;
		IDXGISwapChain* m_SwapChain;

		ID3D11RenderTargetView* m_RenderTarget; /*these things are to fix some problems in games
												 of imgui not rendering or the alpha getting broken*/
		ID3D11BlendState* m_BlendState;
		float m_BlendColor[4];

	}; extern std::shared_ptr<Renderer> g_Renderer;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND Hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);