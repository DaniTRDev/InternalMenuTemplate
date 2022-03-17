#pragma once

namespace change_me
{
	class UIConfiguration /*: public ConfigurationBase*/
	{
	public:

		/*void OnSave() override
		{
			write everything to m_InternalJson
		}
		void OnGet() override
		{
			get everything from m_InternalJson;
		}

		void OnGetConfig(std::string_view Name) override
		{
			return m_InternalJson[Name].As<ConfigurationBase>();
		}*/

	public:

		ImVec2 m_MenuPos{120.f, 100.f};
		float m_MenuWidth{ 280.f }; /*max width, used to wrap*/
		ImVec2 m_MenuMargin{ 20.f, 20.f }; /*used to spaciate things outside the menu*/

		std::uint32_t m_MenuOutlineColor{ IM_COL32(0, 232, 178, 255) };
		float m_MenuOutlineScale{ 1.f };

		std::uint32_t m_MenuScrollColor{ IM_COL32(0, 232, 178, 255) };
		float m_MenuScrollSpeed{ 0.2f };

		std::uint32_t m_MenuScrollBarColor{ IM_COL32(54, 54, 54, 190) };
		std::uint32_t m_MenuScrollBarScrollerColor{ IM_COL32(0, 232, 178, 255) }; /*the shit that indicates the selected opt*/
		float m_MenuScrollBarSpeed{ 0.2f };
		float m_MenuScrollBarWidth{ 10.f };

		ImVec2 m_OptMargin { 15.f, 8.f }; /*the when rendering option text our widgets:
							DrawPos = ImVec(m_MenuPos.x + m_OptMargin.x, m_MenuPos.y + m_OptMargin.y)*/

		std::uint32_t m_OptionBoxColor{ IM_COL32(54, 54, 54, 190) };
		std::uint32_t m_OptionTextColor{ IM_COL32(255, 255, 255, 255) };

		float m_OptionTextSize{ 1.f } /*this will replace ImFont::Scale*/;

		float m_WidgetMargin { 2.f }; /*space between widgets*/

		std::size_t m_MaxVisOptions{ 16 };

		Timer m_DownKeyTimer { 200ms };
		Timer m_UpKeyTimer { 200ms };

		/*nlohman::json m_InternalJson;*/

	};

	/*instead of std::shared_ptr<SubMenu> or std::shared_ptr<Option> for UIManager methods
	we use raw pointers to avoid creating another reference to the shared pointer to be able to properly
	uninitialize it*/

	class UIManager
	{
	public:

		//UIManager(std::shared_ptr<Configuration> Config);*/

		void Render();
		void UpdateDeltaTime();

		void RenderScroller(const ImVec2& Pos, const ImVec2& Size);
		void RenderOption(Option* Opt, ImVec2& Pos, bool CurrentOption); /*Pos != const because we need to modify it*/

		void RenderScrollBar(const ImVec2& StartPos, const ImVec2& EndPos, std::size_t SelectedOptionId, std::size_t NumOptions);

		void ScrollForward(SubMenu * Sub);
		void ScrollBackward(SubMenu* Sub);

		void AddSub(std::string_view Name, std::function<void(SubMenu*)> Cb);

		void HandleInput(SubMenu* Sub);
		void OnInput(UINT Msg, std::uint32_t Key, LPARAM lParam);

		void Uninitialize();

	private:

		std::queue<std::uint32_t> m_SubMenuIndexes; /*used to be able to get to the previous sub*/
		std::map<std::uint32_t, std::shared_ptr<SubMenu>> m_SubMenus; 

		UIConfiguration m_Cfg; /*when config system is added make a unique_ptr of this shit*/

		float m_LastDelta;
		float m_CurrentDelta;

		float m_DeltaTime;

		/*std::shared_ptr<Configuration> Config;*/
		bool m_Up; /*used to do scrolling*/
		bool m_Down; /*used to do scrolling*/
	};
}

