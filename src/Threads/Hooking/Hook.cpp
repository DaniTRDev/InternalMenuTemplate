#include "Common.hpp"
#include "Threads/Pointers/Pointers.hpp"
#include "Hooking.hpp"

namespace change_me
{
	void AddHook(std::shared_ptr<Hook> Hk)
	{
		g_Hooking->m_Hooks.push_back(Hk);
	}
}