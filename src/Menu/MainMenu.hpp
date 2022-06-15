#pragma once

#include "Self/Self.hpp"

namespace change_me
{
	struct MainMenu : public Singleton<MainMenu>
	{
		void Render();
	};
}