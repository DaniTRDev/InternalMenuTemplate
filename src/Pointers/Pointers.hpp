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

		ID3D11Device* m_Device;
		ID3D11DeviceContext* m_Context;

		using Present_t = HRESULT(IDXGISwapChain* This, UINT Flags, UINT VSync);
		HookWrapper<DHookWrapper, Present_t*> m_SwapChain;

		using FrameTick_t = void(__int64 ThisPtr, void* a1, const float tick, float fps, wchar_t** gfxInfo, int mousePos[2]);
		HookWrapper<DHookWrapper, FrameTick_t*> m_FrameTick;/*gfxtest*/

		HWND m_Hwnd;

	private:

	};
}
