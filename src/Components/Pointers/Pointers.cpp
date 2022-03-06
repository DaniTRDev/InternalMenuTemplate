#include "Common.hpp"
#include "Pointers.hpp"
#include "../Hooking/Hooking.hpp"

namespace change_me
{
	std::shared_ptr<Pointers> g_Pointers;
	Pointers::Pointers() : ComponentBase("Pointers", ComponentType::NoNeedsTick)
		{}

	bool Pointers::Initialize()
	{
		g_Pointers = g_ComponentMgr->GetComponent<Pointers>("Pointers");

		m_Test1 = new HookWrapper<DHookWrapper, WndProc_t*>(
			"40 55 53 56 57 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 10", "Test1",
			(void*)&WndProcHK);
		/*
		to adjust the ptr use 
			m_Test1->OnAdjustPtr = [&](PointerMath& Ptr) { Ptr = Ptr.ReadRel(1).Add(5); };

		to get a variable from this ptr: (you have a class with a variable that allows you to get another variable 
		which is important); Ptr already adjusted

			m_Test1->OnGetPtrsFromPtr = [&](PointerMath& Ptr) { this->m_MyVar = Ptr.As<ClassName*>()->GetVar(); };

		*/
		
		m_Device = new PatternImpl<ID3D11Device*>("48 8B 0D ? ? ? ? 8B D8 85 DB", "Device");
		m_Device->OnAdjustPtr = [this](PointerMath& Ptr)
		{
			Ptr = Ptr.ReadRel(3);
			Ptr = PointerMath(std::uintptr_t(*Ptr.As<ID3D11Device**>()));
			Ptr.As<ID3D11Device*>()->GetImmediateContext(&m_Context);
		};

		m_Height = new PatternImpl<long*>("2B 45 FB 89 05", "WHeight");
		m_Height->OnAdjustPtr = [](PointerMath& Ptr) { Ptr = Ptr.Add(3).ReadRel(2); };

		m_Width = new PatternImpl<long*>("89 05 ? ? ? ? 8B 45 03", "WWidth");
		m_Width->OnAdjustPtr = [](PointerMath& Ptr) { Ptr = Ptr.ReadRel(2); };

		m_Hwnd = FindWindowA("SonySantaMonica", "God of War");

		g_PatternScanner->OnModule(g_ModuleManager->GetModule(g_GameModuleName), [&] {

			g_PatternScanner->Add(m_Test1);
			g_PatternScanner->Add(m_Device);
			g_PatternScanner->Add(m_Height);
			g_PatternScanner->Add(m_Width);
		});

		m_Initialized = true;

		return true;
	}
	bool Pointers::Run()
	{
		return true;
	}
	bool Pointers::Uninitialize()
	{
		m_Initialized = false;
		return true;
	}
}