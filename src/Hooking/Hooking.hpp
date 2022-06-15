#pragma once

namespace change_me
{
	/*moved HookingBase to another file for a cleaner hooking files*/

	extern HRESULT WndProcHK(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	extern void FrameTickHK(__int64 ThisPtr, void* a1, const float tick, float fps, wchar_t** gfxInfo, int mousePos[2]);

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

		std::recursive_mutex m_Mutex;

	};
}