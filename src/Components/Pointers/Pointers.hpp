#pragma once

/*don't add this one to Common.hpp because it will be constantly modified*/
namespace change_me
{
	class Pointers : public ComponentBase
	{
	public:

		Pointers();

		bool Initialize() override;
		bool Run() override;
		bool Uninitialize() override;

		using WndProc_t = HRESULT(HWND, UINT, WPARAM, LPARAM);
		HookWrapper<DHookWrapper, WndProc_t*>* m_Test1;

		PatternImpl<ID3D11Device*>* m_Device;
		ID3D11DeviceContext * m_Context;

		PatternImpl<long*>* m_Height;
		PatternImpl<long*>* m_Width;

		HWND m_Hwnd;

	private:

	}; extern std::shared_ptr<Pointers> g_Pointers;
}
