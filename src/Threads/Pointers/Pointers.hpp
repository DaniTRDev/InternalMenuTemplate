#pragma once

/*don't add this one to Common.hpp because it will be constantly modified*/
namespace change_me
{
	class Pointers : public ThreadPoolBase
	{
	public:

		Pointers();

		bool Initialize();
		bool Run();

		void UnitializeThread() override;

		using WndProc_t = HRESULT(HWND, UINT, WPARAM, LPARAM);
		HookWrapper<DHookWrapper, WndProc_t*> m_Test1;

		PointerImpl<ID3D11Device*> m_Device;
		PointerImpl<ID3D11DeviceContext*> m_Context;

		using Present_t = HRESULT(IDXGISwapChain* This, UINT Flags, UINT VSync);
		HookWrapper<DHookWrapper, Present_t*> m_SwapChain;

		PointerImpl<long*> m_Height;
		PointerImpl<long*> m_Width;

		HWND m_Hwnd;

	private:

	}; extern std::shared_ptr<Pointers> g_Pointers;
}
