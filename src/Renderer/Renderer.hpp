#pragma once

namespace change_me
{

	class Renderer : public Singleton<Renderer>
	{
	public: /*general render methods*/

		Renderer();

		void Initialize(ID3D11Device* Device, ID3D11DeviceContext* DeviceContext, IDXGISwapChain* SwapChain);
		void Uninitialize();

	public: /*scene stuff*/

		void NewFrame();
		void Render();
		void EndFrame();

		bool WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	public: /*d3d related stuff*/

		bool CreateRenderTarget();
		bool CreateBlendState();

		ID3D11Texture2D* Create2DTexture(D3D11_TEXTURE2D_DESC * Desc, D3D11_SUBRESOURCE_DATA* ResourceData);
		ID3D11ShaderResourceView* CreateShaderResourceView(ID3D11Texture2D* Texture, D3D11_SHADER_RESOURCE_VIEW_DESC* Desc);

		ID3D11ShaderResourceView* CreateShaderResourceViewForImg(void* Pixels, std::uint32_t Width, std::uint32_t Height, std::string ImgName);

		void PreResize();
		void PostResize();

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetContext();

		IDXGISwapChain* GetSwapChain();

	private:

		void InitializeImGui();

	private:

		bool m_Open;
		bool m_Initialized;

	private: /*d3d11 stuff*/

		ID3D11Device* m_Device; /*we save a local copy to make our live easier while coding*/
		ID3D11DeviceContext* m_DeviceContext;
		IDXGISwapChain* m_SwapChain;

		ID3D11RenderTargetView* m_RenderTarget; /*these things will fix some problems in games
												 of imgui not rendering or the alpha getting broken*/
		ID3D11BlendState* m_BlendState;
		float m_BlendColor[4];

	};
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND Hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);