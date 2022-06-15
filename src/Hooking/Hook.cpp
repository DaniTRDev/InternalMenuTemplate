#include "Common.hpp"
#include "Pointers/Pointers.hpp"
#include "Hooking.hpp"

namespace change_me
{
	void AddHook(std::shared_ptr<Hook> Hk)
	{
		Hooking::Get().m_Hooks.push_back(Hk);
	}
}