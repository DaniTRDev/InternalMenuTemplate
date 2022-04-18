#pragma once

namespace change_me
{
	/*moved HookingBase to another file for a cleaner hooking files*/

	extern HRESULT WndProcHK(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	constexpr std::size_t g_SwapChainNumMethods = 19;
	constexpr std::size_t g_SwapChainPresentId = 8;
	extern HRESULT PresentHK(IDXGISwapChain * ThisPtr, UINT Flags, UINT VSync);

	class Hooking : public Singleton<Hooking>
	{
	public:

		void DoHooks();
		void Unitialize();

	public:

		std::vector<std::shared_ptr<Hook>> m_Hooks;

	};
}