#pragma once

namespace change_me
{
	struct UIIcon
	{
		UIIcon(std::string Name, ID3D11ShaderResourceView* ResourceView, ImVec2 IconSize, 
			void * IconPixelBuffer, std::uint32_t Channels);
		~UIIcon();

		std::string m_Name;
		ID3D11ShaderResourceView* m_ResourceView;

		ImVec2 m_IconSize; /*used to properly scale it*/

		void* m_IconPixelBuffer;/*we keep the buffer to not need to reload the icon everytime we resize it*/
		std::uint32_t m_Channels;

		ImVec2 m_ScaledIconSize;
		void* m_ScaledIconPixelBuffer;

	};
	struct UIFont
	{
		UIFont(std::string Name, ImFont* Font);
		~UIFont();

		std::string m_Name;
		ImFont* m_ImFont;
	};

	struct UITab
	{
		UITab(std::string_view Name, std::uint32_t IconId, std::uint32_t FontId);

		std::string_view m_Name;

		std::uint32_t m_IconId;
		std::uint32_t m_FontId;
	};

	class UIManager
	{
	public:

		UIManager();

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

		bool AddTab(std::string_view Name, std::uint32_t IconId, std::uint32_t FontId);
		void PopTab();

		void ScaleIcon(std::shared_ptr<UIIcon> Icon, ImVec2 NewSize); 
		/*this will be called after laoding icons or after changing options from TabSelector*/
		
	public:

		ImVec2 GetSingleTabSizeM(); /*margins applied*/
		ImVec2 GetTabSelectorSize();

		ImVec2 GetTabCoords(std::size_t TabIndex); /*relative from the box starting pos*/

	private:

		bool m_SwitchingTab;
		bool m_SwitchTabAnimState;

		std::uint32_t m_CurrentTabId { 0 };

		std::map<std::uint32_t, std::unique_ptr<UITab>> m_Tabs;

		std::map<std::uint32_t, std::shared_ptr<UIIcon>> m_Icons;
		std::map<std::uint32_t, std::shared_ptr<UIFont>> m_Fonts;

		bool m_PushedFont;

	public: /*configurations, this will be moved into another struct / class when the config system is done*/
		
		Folder m_UIFolder;

		ImVec2 m_TabSelectorPos{ 0.f, 0.f };

		ImVec2 m_TabSelectorSize { 25.f, 30.f };/*raw size per tab - NO MARGINS*/
		ImVec2 m_TabSelectorMargin { 5.f, 5.f }; /*the space between the icon and the rect end*/

		std::uint32_t m_TabSelectorBoxCol{ IM_COL32(26, 23, 30, 255) };
		std::uint32_t m_TabSelectorSelectedTabCol{ IM_COL32(72, 76, 167, 255) };
		std::uint32_t m_TabSelectorHoveredTabCol{ IM_COL32(72, 76, 167, 180) };
		std::uint32_t m_TabSelectorOutlineColor{ IM_COL32(72, 76, 167, 255) };
	};
}

