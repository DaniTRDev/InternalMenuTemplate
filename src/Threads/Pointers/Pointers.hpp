#pragma once

/*don't add this one to Common.hpp because it will be constantly modified*/
namespace change_me
{

#define POINTER_DECL(Ptr, Name, PointerAddr, Adjust) PointerAddr = PointerAddr.Adjust; \
	Ptr = decltype(Ptr)(Name, PointerAddr);

#define POINTER_DECL_RAW(Ptr, Name, Addr) Ptr = decltype(Ptr)(Name, std::uintptr_t(Addr));

#define DHOOK_DECL(Ptr, Name, Detour, PointerAddr, Adjust) PointerAddr = PointerAddr.Adjust; \
	 Ptr = decltype(Ptr)(Name, PointerAddr.As<std::uintptr_t>(), (void*)Detour);

#define DHOOK_DECL_RAW(Ptr, Name, Detour, Addr) Ptr = decltype(Ptr)(Name, std::uintptr_t(Addr), (void*)Detour);


	class Pointers : public Singleton<Pointers>
	{
	public:

		Pointers();

		bool Initialize();

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

	};
}
