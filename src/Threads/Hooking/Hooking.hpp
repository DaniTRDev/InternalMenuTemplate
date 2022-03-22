#pragma once

namespace change_me
{
	/*moved HookingBase to another file for a cleaner hooking files*/

	extern HRESULT WndProcHK(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	constexpr std::size_t g_SwapChainNumMethods = 19;
	constexpr std::size_t g_SwapChainPresentId = 8;
	extern HRESULT PresentHK(IDXGISwapChain * ThisPtr, UINT Flags, UINT VSync);

	class Hooking : public ThreadPoolBase
	{
	public:

		Hooking();

		bool Initialize();
		bool Run();

		void UnitializeThread() override;

	public:

		std::vector<std::shared_ptr<Hook>> m_Hooks;

	}; extern std::shared_ptr<Hooking> g_Hooking;
}