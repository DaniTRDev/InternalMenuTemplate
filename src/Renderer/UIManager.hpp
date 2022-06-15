#pragma once

namespace change_me
{
	struct UIIcon
	{
		UIIcon(ID3D11ShaderResourceView* ResourceView, ImVec2 IconSize, 
			void * IconPixelBuffer, std::uint32_t Channels);
		~UIIcon();

		ID3D11ShaderResourceView* m_ResourceView;

		ImVec2 m_IconSize; /*used to properly scale it*/

		void* m_IconPixelBuffer;/*we keep the buffer to not need to reload the icon everytime we resize it*/
		std::uint32_t m_Channels;

		ImVec2 m_ScaledIconSize;
		void* m_ScaledIconPixelBuffer;

	};
	struct UIFont
	{
		UIFont(ImFont* Font);
		~UIFont();

		ImFont* m_ImFont;
	};

	struct UITabConfig
	{
		std::string m_Name;
		std::string m_DisplayName;
		std::string m_FontName; /*if these 2 fields are empty the it will use default font & icon*/
		std::string m_IconName;
	};
	struct UITab
	{
		UITab(std::string_view DisplayName, std::shared_ptr<UIIcon> Icon, std::shared_ptr<UIFont> Font);

		std::string m_DisplayName;

		bool m_Selected{};
		bool m_Show{};

		std::shared_ptr<UIIcon> m_Icon;
		std::shared_ptr<UIFont> m_Font;
	};

	class UIManager : public Singleton<UIManager>, public SettingsListener
	{
	public:

		UIManager();

		void Save() override;
		void Load(nlohmann::json& Node) override;

		void Initialize(); /*load fonts and icons*/
		void Uninitialize();

		void LoadStyle();
		void LoadIcons();
		void LoadFonts();

		void ReloadStyle();
		void ReloadIcons();
		void ReloadFonts();

		std::shared_ptr<UIIcon> LoadUIIcon(const std::filesystem::path& FilePath);
		std::shared_ptr<UIIcon> LoadUIIconFromMemory(std::string IconName, 
			void* Pixels, std::uint32_t Width, std::uint32_t Height, std::uint32_t Channels);

		std::shared_ptr<UIFont> LoadUIFont(const std::filesystem::path& FilePath);

		void Render(); /*render the current tab*/
		void RenderTabSelector(); /*render the tab selector*/

		void UpdateInput();

		bool AddTab(std::string_view Name);
		void PopTab();

		void ScaleIcon(std::string_view Name, std::shared_ptr<UIIcon> Icon, ImVec2 NewSize); 
		/*this will be called after laoding icons or after changing options from TabSelector*/

	public:

		ImVec2 GetSingleTabSizeM(); /*margins applied*/
		ImVec2 GetTabSelectorSize();

		ImVec2 GetTabCoords(std::size_t TabIndex); /*relative from the box starting pos*/

	private:

		void OnNewTab(std::string_view Name);

	private:

		bool m_SwitchingTab;
		bool m_SwitchTabAnimState;

		std::uint32_t m_LastSelectedTab; /*used to control the selecting tab animation*/

		std::map<std::string, std::shared_ptr<UITab>> m_Tabs;
		std::vector<std::shared_ptr<UITab>> m_SelectorTabs; /*used to have a quick acces to tabs 
																		from the tab selector*/

		std::map<std::string, std::shared_ptr<UIIcon>> m_Icons;
		std::map<std::string, std::shared_ptr<UIFont>> m_Fonts;

		std::shared_ptr<UIIcon> m_DefaultTabIcon; /*used to avoid crash when a tab has an invalid icon name*/
		std::shared_ptr<UIFont> m_DefaultTabFont; /*used to avoid crash when a tab has an invalid font name*/

	private: /*configurations, this will be moved into another struct / class when the config system is done*/
		
		Folder m_UIFolder;

		SettingArray<UITabConfig> m_TabsCfg{ "Tabs" };

		Setting<ImVec2, ImVec2(0.f, 0.f)> m_TabSelectorPos{ "TabSelectorPos" };

		Setting<ImVec2, ImVec2(25.f, 30.f)> m_TabSelectorSize{ "TabSelectorSize" };/*raw size per tab - NO MARGINS*/
		Setting<ImVec2, ImVec2(5.f, 5.f)> m_TabSelectorMargin{ "TabSelectorMargin" }; /*the space between the icon and the rect end*/

		Setting<ImVec4, ImVec4(26, 23, 30, 255)> m_TabSelectorBoxCol{ "TabSelectorBoxCol" };
		Setting<ImVec4, ImVec4(72, 76, 167, 255)> m_TabSelectorSelectedTabCol{ "TabSelectorSelectedTabCol" };
		Setting<ImVec4, ImVec4(72, 76, 167, 180)> m_TabSelectorHoveredTabCol{ "TabSelectorHoveredTabCol" };
		Setting<ImVec4, ImVec4(72, 76, 167, 255)> m_TabSelectorOutlineColor{ "TabSelectorOutlineColor" };

	};
}

