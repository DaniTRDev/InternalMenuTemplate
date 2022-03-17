#include "Common.hpp"

namespace change_me
{

	SubMenu::SubMenu(std::string_view SubName, std::function<void(SubMenu*)> Cb) : m_Name(SubName), 
		m_Hash(Joaat(SubName)),  m_CurrentOpt(0), m_Callback(Cb)
		{}

	const std::vector<std::shared_ptr<Option>>& SubMenu::GetOptions()
	{
		return m_Options;
	}
	const std::size_t& SubMenu::GetCurrentOpt()
	{
		return m_CurrentOpt;
	}
	
	const std::uint32_t& SubMenu::GetHash()
	{
		return m_Hash;
	}

	void SubMenu::AddOption(std::string_view Text, std::function<void(OptionParams*)> Cb)
	{
		m_Options.push_back(std::make_shared<Option>(Text, Cb));
	}

	void SubMenu::NewFrame()
	{
		m_Callback(this);
	}
	void SubMenu::EndFrame()
	{
		m_Options.clear();
	}

	void SubMenu::SetSelectedOption(std::size_t NewOpt)
	{
		m_CurrentOpt = NewOpt;
	}
}