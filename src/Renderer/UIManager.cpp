#include "Common.hpp"
#include "UIManager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_STATIC
#include <stb_image_resize.h>

namespace change_me
{
	UIIcon::UIIcon(std::string Name, ID3D11ShaderResourceView* ResourceView, ImVec2 IconSize,
		void* IconPixelBuffer, std::uint32_t Channels) :
		m_Name(Name), m_ResourceView(ResourceView), m_IconSize(IconSize), m_IconPixelBuffer(IconPixelBuffer), m_Channels(Channels)
	{
	}
	UIIcon::~UIIcon()
	{
		m_ResourceView->Release();
		stbi_image_free(m_IconPixelBuffer);
	}

	UIFont::UIFont(std::string Name, ImFont* Font) : m_Name(Name), m_ImFont(Font)
	{
	}
	UIFont::~UIFont()
	{
		m_ImFont->ClearOutputData();
	}

	UITab::UITab(std::string_view Name, std::uint32_t IconId, std::uint32_t FontId) : m_Name(Name), m_IconId(IconId),
		m_FontId(FontId)
	{
	}

	UIManager::UIManager() : m_UIFolder(FileManager::Get()->GetProjectFolder("UI")), m_SwitchingTab(false)
	{
	}

	void UIManager::Initialize()
	{
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
		for (auto& Entry : std::filesystem::directory_iterator(m_UIFolder.GetPath()))
		{
			if (!Entry.is_regular_file())
				continue;

			if (Entry.path().extension() == ".png" || Entry.path().extension() == ".jpg"
				|| Entry.path().extension() == ".jpeg")
			{
				if (auto Icon = LoadUIIcon(Entry.path()); Icon)
					m_Icons.insert({ std::uint32_t(m_Icons.size()), Icon });
				
			}
		}
	}
	void UIManager::LoadFonts()
	{
		for (auto& Entry : std::filesystem::directory_iterator(m_UIFolder.GetPath()))
		{
			if (!Entry.is_regular_file())
				continue;

			if (Entry.path().extension() == ".ttf")
			{
				if (auto Font = LoadUIFont(Entry.path()); Font)
					m_Fonts.insert({ std::uint32_t(m_Fonts.size()), Font });
				
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
		auto View = Renderer::Get()->CreateShaderResourceViewForImg(Pixels, Width, Height, IconName);
		if (!View)
		{
			LOG(WARNING) << "Could not create icon resource view: [" << ADD_COLOR_TO_TEXT(LogColor::BLUE, IconName ) 
				<< "]";
			return nullptr;
		}

		LOG(INFO) << "Created new icon: [" << ADD_COLOR_TO_TEXT(LogColor::BLUE, IconName )
			<< "] -> " << "Size { Width: " << Width << " Height: " << Height << "}";

		auto Icon = std::make_shared<UIIcon>(IconName, View, ImVec2(float(Width), float(Height)), Pixels, Channels);
		ScaleIcon(Icon, m_TabSelectorSize);

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
		return std::make_shared<UIFont>(FilePath.filename().string(), ImFont);
	}

	void UIManager::Render()
	{
		m_TabSelectorPos = ImVec2((ImGui::GetIO().DisplaySize.x / 2.f) - (GetTabSelectorSize().x / 2.f), 7.f);

		RenderTabSelector();
		UpdateInput();

		if (m_SwitchingTab && m_SwitchTabAnimState)
		{
			g_AnimationManager->PushNewAnimation(std::make_shared<AnimationBase>([this](float Delta)
				{
					auto DrawList = ImGui::GetWindowDrawList();
					auto CircleRadious = (m_TabSelectorSize.y / 2.f) *
						((Delta > 0.9f) ? (1.f) : (Delta));

					auto BoxPos = m_TabSelectorPos;
					auto NewTabCoords = BoxPos + m_TabSelectorMargin + GetTabCoords(m_CurrentTabId);

					NewTabCoords.x += (m_TabSelectorSize.y / 2.f);
					NewTabCoords.y += (m_TabSelectorSize.y / 2.f);

					DrawList->AddCircleFilled(NewTabCoords, CircleRadious, m_TabSelectorSelectedTabCol);

					if (Delta >= 0.9f && Delta <= 1.f)
						m_SwitchingTab = false;

				}, 250ms));

			m_SwitchTabAnimState = false;
		}

		m_Tabs.clear(); /*clear it for the next frame*/
	}
	void UIManager::RenderTabSelector()
	{
		auto DrawList = ImGui::GetWindowDrawList();
		
		auto BoxSize = GetTabSelectorSize();
		auto BoxPos = m_TabSelectorPos;

		/*draw the main box*/
		DrawList->AddRectFilled(BoxPos, BoxPos + BoxSize, m_TabSelectorBoxCol, 5.f, ImDrawFlags_RoundCornersAll);

		for (auto i = 0; i < m_Tabs.size(); i++)
		{
			auto Tab = std::move(m_Tabs[i]);
		
			auto DrawPos = BoxPos + m_TabSelectorMargin + GetTabCoords(i);
			auto IconBoxSize = GetSingleTabSizeM() - (m_TabSelectorMargin * 2); /*remove the size of the margins*/

			/*TO DO: ADD SMOOTH ICON SWITCH*/
			if (i == m_CurrentTabId && !m_SwitchingTab) /*draw the rect for the selected icon*/
				DrawList->AddRectFilled(DrawPos, DrawPos + IconBoxSize, m_TabSelectorSelectedTabCol, 5.f, ImDrawFlags_RoundCornersAll);

			DrawPos += m_TabSelectorMargin;
			IconBoxSize -= (m_TabSelectorMargin * 2);

			DrawList->AddImage(m_Icons[Tab->m_IconId]->m_ResourceView, DrawPos, DrawPos + IconBoxSize);

			DrawPos.x += (IconBoxSize.x + 3 * m_TabSelectorMargin.x); /*add an extra margin to separate tabs, we also need
																  to add the margins of the right side*/
			DrawPos.y = m_TabSelectorMargin.y + 7.f; /*restore DrawPos.y back to the original coord*/

			Tab.release();
		}
	}

	void UIManager::UpdateInput()
	{
		auto DrawList = ImGui::GetWindowDrawList();

		auto SingleTabSizeM = GetSingleTabSizeM();
		auto BoxSize = GetTabSelectorSize();

		auto BoxPos = m_TabSelectorPos;
		auto CursorPos = ImGui::GetMousePos();

		if (ImGui::IsMouseHoveringRect(BoxPos, BoxPos + BoxSize))
		{ /*the coursor is inside the tab selector box*/
			auto HoveredTab = std::uint32_t(std::trunc((CursorPos.x - BoxPos.x) /
				(SingleTabSizeM.x + m_TabSelectorMargin.x)));

			if (m_CurrentTabId == HoveredTab) /*the hovered tab is already selected, no need to waste resources*/
				return;

			auto HoveredTabCoords = BoxPos + m_TabSelectorMargin + GetTabCoords(HoveredTab);
			SingleTabSizeM -= (m_TabSelectorMargin * 2);

			DrawList->AddRectFilled(HoveredTabCoords, HoveredTabCoords + SingleTabSizeM,
				m_TabSelectorHoveredTabCol, 5.f,
				ImDrawFlags_RoundCornersAll);

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_SwitchingTab = true;
				m_SwitchTabAnimState = true;

				m_CurrentTabId = HoveredTab;
			}
		}
	}

	bool UIManager::AddTab(std::string_view Name, std::uint32_t IconId, std::uint32_t FontId)
	{
		if (m_Fonts.find(FontId) != m_Fonts.end())
		{
			ImGui::PushFont(m_Fonts[FontId]->m_ImFont);
			m_PushedFont = true;
		}

		auto Tab = std::make_unique<UITab>(Name, IconId, FontId);
		m_Tabs.insert({ std::uint32_t(m_Tabs.size()), std::move(Tab) });

		return m_CurrentTabId == m_Tabs.size() - 1;
	}
	void UIManager::PopTab()
	{
		if (m_PushedFont)
		{
			ImGui::PopFont();
			m_PushedFont = false;
		}
	}

	void UIManager::ScaleIcon(std::shared_ptr<UIIcon> Icon, ImVec2 NewSize)
	{
		auto OutPixels = new char[std::size_t(NewSize.x * NewSize.y * float(Icon->m_Channels))];

		if (!stbir_resize_uint8((std::uint8_t*)Icon->m_IconPixelBuffer, std::uint32_t(Icon->m_IconSize.x), 
			std::uint32_t(Icon->m_IconSize.y), 0, (std::uint8_t*)OutPixels, 
			std::uint32_t(NewSize.x), std::uint32_t(NewSize.y), 0, Icon->m_Channels))
		{
			LOG(WARNING) << "Could not resize icon: [" << ADD_COLOR_TO_TEXT(LogColor::BLUE, Icon->m_Name) << "]; menu will use the original size";
			return;
		}

		Icon->m_ScaledIconPixelBuffer = OutPixels;
		Icon->m_ScaledIconSize = NewSize;

		auto View = Renderer::Get()->CreateShaderResourceViewForImg(OutPixels,
			std::uint32_t(NewSize.x), std::uint32_t(NewSize.y), Icon->m_Name);

		if (!View)
		{
			LOG(WARNING) << "Could not generate icon rsv: [" << ADD_COLOR_TO_TEXT(LogColor::BLUE, Icon->m_Name ) << "]; menu will use the original size";
			return;
		}

		Icon->m_ResourceView->Release();
		Icon->m_ResourceView = View;
	}

	ImVec2 UIManager::GetSingleTabSizeM()
	{
		return ImVec2(m_TabSelectorSize.x + 4 * m_TabSelectorMargin.x,
			m_TabSelectorSize.y + 4 * m_TabSelectorMargin.y); /*margins applied*/
	}
	ImVec2 UIManager::GetTabSelectorSize()
	{
		auto SingleTabSize = GetSingleTabSizeM();
		return ImVec2(SingleTabSize.x * m_Tabs.size() + (m_Tabs.size() - 1) * m_TabSelectorMargin.x,
			SingleTabSize.y);
	}

	ImVec2 UIManager::GetTabCoords(std::size_t TabIndex)
	{
		auto SingleTabSize = GetSingleTabSizeM();
		return ImVec2(SingleTabSize.x * TabIndex + TabIndex * m_TabSelectorMargin.x, 0.f);
	}

}
