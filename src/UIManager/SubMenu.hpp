#pragma once

namespace change_me
{
	class SubMenu
	{
	public:

		SubMenu(std::string_view SubName, std::function<void(SubMenu*)> Cb);

		const std::vector<std::shared_ptr<Option>>& GetOptions(); /*make sure we don't modify any of this things, we can cause fatal errors*/
		const std::size_t& GetCurrentOpt();
		
		const std::uint32_t& GetHash();

		void AddOption(std::string_view Text, std::function<void(OptionParams*)> Cb); /*make sure we make a copy of the struct */
		
		void NewFrame();
		void EndFrame();

		void SetSelectedOption(std::size_t NewOpt);

	private:

		std::string_view m_Name;
		std::uint32_t m_Hash;
		
		std::size_t m_CurrentOpt;
		std::vector<std::shared_ptr<Option>> m_Options;

		std::function<void(SubMenu*)> m_Callback;
	};

}

