#include "Common.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_STATIC
#include <stb_image_resize.h>

namespace change_me
{
	UIIcon::UIIcon(ID3D11ShaderResourceView* ResourceView, ImVec2 IconSize,
		void* IconPixelBuffer, std::uint32_t Channels) :
		m_ResourceView(ResourceView), m_IconSize(IconSize), m_IconPixelBuffer(IconPixelBuffer), m_Channels(Channels)
	{
	}
	UIIcon::~UIIcon()
	{
		m_ResourceView->Release();
		stbi_image_free(m_IconPixelBuffer);
	}

	UIFont::UIFont(ImFont* Font) : m_ImFont(Font)
	{
	}
	UIFont::~UIFont()
	{
		m_ImFont->ClearOutputData();
	}

	UITab::UITab(std::string_view DisplayName, std::shared_ptr<UIIcon> Icon, std::shared_ptr<UIFont> Font) :
		m_DisplayName(DisplayName), m_Selected(false), m_Show(false), m_Icon(Icon), m_Font(Font)
	{
	}

	UIManager::UIManager() : m_UIFolder(FileManager::Get().GetProjectFolder("UI")), m_SwitchingTab(false), 
		SettingsListener("")
	{
	}

	void UIManager::Save()
	{
		m_TabsCfg.Save(m_Buffer);

		m_TabSelectorPos.Save(m_Buffer);

		m_TabSelectorSize.Save(m_Buffer);
		m_TabSelectorMargin.Save(m_Buffer);

		m_TabSelectorBoxCol.Save(m_Buffer);
		m_TabSelectorSelectedTabCol.Save(m_Buffer);
		m_TabSelectorHoveredTabCol.Save(m_Buffer);
		m_TabSelectorOutlineColor.Save(m_Buffer);
	}
	void UIManager::Load(nlohmann::json& Node)
	{
		m_TabsCfg.Load(Node);

		m_TabSelectorPos.Load(Node);

		m_TabSelectorSize.Load(Node);
		m_TabSelectorMargin.Load(Node);

		m_TabSelectorBoxCol.Load(Node);
		m_TabSelectorSelectedTabCol.Load(Node);
		m_TabSelectorHoveredTabCol.Load(Node);
		m_TabSelectorOutlineColor.Load(Node);
	}

	void UIManager::Initialize()
	{
		g_UISettings = std::make_shared<SettingsManager>(FileManager::Get().GetProjectFolder("config").GetFile("UIConfig.json").GetPath());
		g_UISettings->AddListener(UIManager::GetPtr());
		g_UISettings->Load();

		LoadStyle();
		LoadIcons();
		LoadFonts();
	}
	void UIManager::Uninitialize()
	{
		LOG(INFO) << "Unitializing UIManager";

		m_Icons.clear(); /*we don't need to free the resource inside icons or fonts because std::shared_ptr will call
						 the destructor and it will free these resources*/
		m_Fonts.clear();
		m_Tabs.clear();
	}

	void UIManager::LoadStyle()
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		style->WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style->WindowPadding = ImVec2(15, 15);
		style->WindowRounding = 5.0f;
		style->ChildRounding = 5.0f;
		style->PopupRounding = 5.0f;
		style->FramePadding = ImVec2(5, 5);
		style->FrameRounding = 4.0f;
		style->ItemSpacing = ImVec2(12, 8);
		style->ItemInnerSpacing = ImVec2(8, 6);
		style->IndentSpacing = 25.0f;
		style->ScrollbarSize = 5.0f;
		style->ScrollbarRounding = 0.0f;
		style->GrabMinSize = 5.0f;
		style->GrabRounding = 3.0f;

		style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_ChildBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_PopupBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		style->Colors[ImGuiCol_FrameBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrab] = ImColor(89, 96, 237, 255);
		style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(89, 96, 237, 255);
		style->Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(89, 96, 237, 255);
		style->Colors[ImGuiCol_CheckMark] = ImColor(89, 96, 237, 255);
		style->Colors[ImGuiCol_SliderGrab] = ImColor(89, 96, 237, 255);
		style->Colors[ImGuiCol_SliderGrabActive] = ImColor(89, 96, 237, 255);
		style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_Separator] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.60f, 0.00f, 0.43f);
		style->Colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_TabActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	}
	void UIManager::LoadIcons()
	{
		m_DefaultTabIcon = LoadUIIcon(m_UIFolder.GetPath() / "default.png");

		for (auto& Entry : std::filesystem::directory_iterator(m_UIFolder.GetPath()))
		{
			if (!Entry.is_regular_file())
				continue;

			for (auto& TabCfg : m_TabsCfg)
			{
				if (TabCfg.m_IconName == Entry.path().filename())
					m_Icons[TabCfg.m_IconName] = LoadUIIcon(Entry.path());
			}
		}
	}
	void UIManager::LoadFonts()
	{
		m_DefaultTabFont = LoadUIFont(m_UIFolder.GetPath() / "default.ttf");

		for (auto& Entry : std::filesystem::directory_iterator(m_UIFolder.GetPath()))
		{
			if (!Entry.is_regular_file())
				continue;

			for (auto& TabCfg : m_TabsCfg)
			{
				if (TabCfg.m_FontName == Entry.path().filename())
					m_Fonts[TabCfg.m_FontName] = LoadUIFont(Entry.path());
			}
		}
	}

	void UIManager::ReloadStyle()
	{ /*pending to add the theme loader*/
		LoadStyle();
	}
	void UIManager::ReloadIcons()
	{
		m_Icons.clear();
		LoadIcons();
	}
	void UIManager::ReloadFonts()
	{
		m_Fonts.clear();
		LoadFonts();
	}

	std::shared_ptr<UIIcon> UIManager::LoadUIIcon(const std::filesystem::path& FilePath)
	{
		auto ImgWidth = 0;
		auto ImgHeight = 0;

		auto Channels = 0;

		auto Pixels = stbi_load(FilePath.string().c_str(), &ImgWidth, &ImgHeight, &Channels, 4);
		if (Pixels == nullptr)
		{
			LOG(WARNING) << "Could not create icon: [" << ADD_COLOR_TO_TEXT(LogColor::BLUE, FilePath.filename() ) 
				<< "], stbi could not load the image, path: " << FilePath.string();
			return nullptr;
		}

		return LoadUIIconFromMemory(FilePath.filename().string(), Pixels, ImgWidth, ImgHeight, Channels);
	}
	std::shared_ptr<UIIcon> UIManager::LoadUIIconFromMemory(std::string IconName, void* Pixels, std::uint32_t Width,
		std::uint32_t Height, std::uint32_t Channels)
	{
		auto View = Renderer::Get().CreateShaderResourceViewForImg(Pixels, Width, Height, IconName);
		if (!View)
		{
			LOG(WARNING) << "Could not create icon resource view: [" << ADD_COLOR_TO_TEXT(LogColor::BLUE, IconName ) 
				<< "]";
			return nullptr;
		}

		LOG(INFO) << "Created new icon: [" << ADD_COLOR_TO_TEXT(LogColor::BLUE, IconName )
			<< "] -> " << "Size { Width: " << Width << " Height: " << Height << "}";

		auto Icon = std::make_shared<UIIcon>(View, ImVec2(float(Width), float(Height)), Pixels, Channels);
		ScaleIcon(IconName.data(), Icon, m_TabSelectorSize.Get());

		return Icon;
	}

	std::shared_ptr<UIFont> UIManager::LoadUIFont(const std::filesystem::path& FilePath)
	{
		auto ImFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(FilePath.string().c_str(), 20.f);

		if (!ImFont)
		{
			LOG(WARNING) << "Could not create font: [" << ADD_COLOR_TO_TEXT(LogColor::BLUE, FilePath.filename() ) 
				<< "], ImGui could not load the font, path: " << FilePath.string();
			return nullptr;
		}

		LOG(INFO) << "Created new font: [" << ADD_COLOR_TO_TEXT(LogColor::BLUE, FilePath.filename() ) << "]";
		return std::make_shared<UIFont>(ImFont);
	}

	void UIManager::Render()
	{
		m_TabSelectorPos = ImVec2((ImGui::GetIO().DisplaySize.x / 2.f) - (GetTabSelectorSize().x / 2.f), 7.f);

		RenderTabSelector();
		UpdateInput();

		if (m_SwitchingTab && m_SwitchTabAnimState)
		{
			AnimationManager::Get().PushNewAnimation(std::make_shared<AnimationBase>([this](float Delta)
				{
					auto DrawList = ImGui::GetWindowDrawList();
					auto CircleRadious = (m_TabSelectorSize.Get().y / 2.f) *
						((Delta > 0.9f) ? (1.f) : (Delta));

					auto BoxPos = m_TabSelectorPos.Get();
					auto NewTabCoords = BoxPos + m_TabSelectorMargin.Get() + GetTabCoords(m_LastSelectedTab);

					NewTabCoords.x += (m_TabSelectorSize.Get().y / 2.f);
					NewTabCoords.y += (m_TabSelectorSize.Get().y / 2.f);

					DrawList->AddCircleFilled(NewTabCoords, CircleRadious, IMVEC4_2_COL32(m_TabSelectorSelectedTabCol.Get()));

					if (Delta >= 0.9f && Delta <= 1.f)
						m_SwitchingTab = false;

				}, 250ms));

			m_SwitchTabAnimState = false;
		}
	}
	void UIManager::RenderTabSelector()
	{
		auto DrawList = ImGui::GetWindowDrawList();
		
		auto BoxSize = GetTabSelectorSize();
		auto BoxPos = m_TabSelectorPos.Get();

		/*draw the main box*/
		DrawList->AddRectFilled(BoxPos, BoxPos + BoxSize, IMVEC4_2_COL32(m_TabSelectorBoxCol.Get()), 5.f, ImDrawFlags_RoundCornersAll);

		for (auto i = 0; i < m_Tabs.size(); i++)
		{
			auto Tab = m_SelectorTabs[i];
		
			auto DrawPos = BoxPos + m_TabSelectorMargin.Get() + GetTabCoords(i);
			auto IconBoxSize = GetSingleTabSizeM() - (m_TabSelectorMargin.Get() * 2); /*remove the size of the margins*/

			/*TO DO: ADD SMOOTH ICON SWITCH*/
			if (Tab->m_Selected && !m_SwitchingTab) /*draw the rect for the selected icon*/
				DrawList->AddRectFilled(DrawPos, DrawPos + IconBoxSize, IMVEC4_2_COL32(m_TabSelectorSelectedTabCol.Get()), 5.f, ImDrawFlags_RoundCornersAll);

			DrawPos += m_TabSelectorMargin.Get();
			IconBoxSize -= (m_TabSelectorMargin.Get() * 2);

			DrawList->AddImage(Tab->m_Icon->m_ResourceView, DrawPos, DrawPos + IconBoxSize);

			DrawPos.x += (IconBoxSize.x + 3 * m_TabSelectorMargin.Get().x); /*add an extra margin to separate tabs, we also need
																  to add the margins of the right side*/
			DrawPos.y = m_TabSelectorMargin.Get().y + 7.f; /*restore DrawPos.y back to the original coord*/
		}
	}

	void UIManager::UpdateInput()
	{
		auto DrawList = ImGui::GetWindowDrawList();

		auto SingleTabSizeM = GetSingleTabSizeM();
		auto BoxSize = GetTabSelectorSize();

		auto BoxPos = m_TabSelectorPos;
		auto CursorPos = ImGui::GetMousePos();

		if (ImGui::IsMouseHoveringRect(BoxPos.Get(), BoxPos + BoxSize))
		{ /*the coursor is inside the tab selector box*/
			auto HoveredTabId = std::uint32_t(std::trunc((CursorPos.x - BoxPos.Get().x) /
				(SingleTabSizeM.x + m_TabSelectorMargin.Get().x)));
			
			auto HoveredTab = m_SelectorTabs[HoveredTabId];

			auto HoveredTabCoords = BoxPos.Get() + m_TabSelectorMargin.Get() + GetTabCoords(HoveredTabId);
			SingleTabSizeM -= (m_TabSelectorMargin.Get() * 2);

			DrawList->AddRectFilled(HoveredTabCoords, HoveredTabCoords + SingleTabSizeM,
				IMVEC4_2_COL32(m_TabSelectorHoveredTabCol.Get()), 5.f,
				ImDrawFlags_RoundCornersAll);

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_SwitchingTab = true;
				m_SwitchTabAnimState = true;
				m_LastSelectedTab = HoveredTabId;

				HoveredTab->m_Selected ^= true; /*select unselected tab and unselect selected tab*/
			
			}
		}
	}

	bool UIManager::AddTab(std::string_view Name)
	{
		if (m_Tabs.count(Name.data()) == 0)
			OnNewTab(Name);

		auto Tab = m_Tabs[Name.data()];
		auto Selected = Tab->m_Selected;
		
		if (Selected)
		{
			ImGui::Begin(Tab->m_DisplayName.data());
			ImGui::PushFont(Tab->m_Font->m_ImFont);
		}

		return Tab->m_Selected;
	}
	void UIManager::PopTab()
	{
		ImGui::PopFont();
		ImGui::End();
	}

	void UIManager::ScaleIcon(std::string_view Name, std::shared_ptr<UIIcon> Icon, ImVec2 NewSize)
	{
		auto OutPixels = new char[std::size_t(NewSize.x * NewSize.y * float(Icon->m_Channels))];

		if (!stbir_resize_uint8((std::uint8_t*)Icon->m_IconPixelBuffer, std::uint32_t(Icon->m_IconSize.x), 
			std::uint32_t(Icon->m_IconSize.y), 0, (std::uint8_t*)OutPixels, 
			std::uint32_t(NewSize.x), std::uint32_t(NewSize.y), 0, Icon->m_Channels))
		{
			LOG(WARNING) << "Could not resize icon: [" << ADD_COLOR_TO_TEXT(LogColor::BLUE, Name) << "]; menu will use the original size";
			return;
		}

		Icon->m_ScaledIconPixelBuffer = OutPixels;
		Icon->m_ScaledIconSize = NewSize;

		auto View = Renderer::Get().CreateShaderResourceViewForImg(OutPixels,
			std::uint32_t(NewSize.x), std::uint32_t(NewSize.y), Name.data());

		if (!View)
		{
			LOG(WARNING) << "Could not generate icon rsv: [" << ADD_COLOR_TO_TEXT(LogColor::BLUE, Name ) << "]; menu will use the original size";
			return;
		}

		Icon->m_ResourceView->Release();
		Icon->m_ResourceView = View;
	}

	ImVec2 UIManager::GetSingleTabSizeM()
	{
		return ImVec2(m_TabSelectorSize.Get().x + 4 * m_TabSelectorMargin.Get().x,
			m_TabSelectorSize.Get().y + 4 * m_TabSelectorMargin.Get().y); /*margins applied*/
	}
	ImVec2 UIManager::GetTabSelectorSize()
	{
		auto SingleTabSize = GetSingleTabSizeM();
		return ImVec2(SingleTabSize.x * m_Tabs.size() + (m_Tabs.size() - 1) * m_TabSelectorMargin.Get().x,
			SingleTabSize.y);
	}

	ImVec2 UIManager::GetTabCoords(std::size_t TabIndex)
	{
		auto SingleTabSize = GetSingleTabSizeM();
		return ImVec2(SingleTabSize.x * TabIndex + TabIndex * m_TabSelectorMargin.Get().x, 0.f);
	}

	void UIManager::OnNewTab(std::string_view Name)
	{
		std::shared_ptr<UIIcon> TabIcon = m_DefaultTabIcon;
		std::shared_ptr<UIFont> TabFont = m_DefaultTabFont;

		std::string DisplayName = "TAB NOT DEFINED IN CONFIGURATION!";

		for (const auto& TabCfg : m_TabsCfg)
		{
			if (TabCfg.m_Name != Name)
				continue;

			if (m_Icons.count(TabCfg.m_IconName) > 0)
				TabIcon = m_Icons.at(TabCfg.m_IconName);

			if (m_Fonts.count(TabCfg.m_FontName) > 0)
				TabFont = m_Fonts.at(TabCfg.m_FontName);

			DisplayName = TabCfg.m_DisplayName;
		}

		auto Tab = std::make_shared<UITab>(DisplayName, TabIcon, TabFont);

		m_Tabs.insert({ Name.data(), Tab});
		m_SelectorTabs.push_back(Tab);
	}
}
