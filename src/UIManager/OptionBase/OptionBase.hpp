#pragma once

namespace change_me
{
	class UIConfiguration;

	class WidgetBase 
	{
	public:

		virtual ImVec2 GetWidgetSize(float WrapWidth) = 0;
		virtual void DrawWidget(ImVec2 Pos, class UIConfiguration Cfg) = 0;

		virtual void UpdateInputs(bool& WantCapture, UINT Msg, std::uint32_t Key, LPARAM lParam) = 0;		
																/*to make the widget know if we want it to handle inputs
																  this bool wont do a shit in the keyboard widget (if the 
																  keyboard is enabled), pass a reference
																  so widgets can modify this to lock the key capturing*/
	}; 

	class OptionParams
	{
	public:
		friend class Option;

		void AddDesc(std::string_view Desc);
		
		template<typename T, typename ...Args>
		inline void AddWidget(Args&&... args)
		{
			/*force a creation of TemplatedWidgetBase*/
			m_Widgets.push_back(std::make_shared<T>(args...));
		}

	private:

		std::string_view m_Desc;
		std::vector<std::shared_ptr<WidgetBase>> m_Widgets;

	};

	class Option
	{
	public:

		Option(std::string_view Text, std::function<void(OptionParams*)> Cb);

		const std::vector<std::shared_ptr<WidgetBase>>& GetWidgets();
		const std::shared_ptr<OptionParams>& GetParams();

		const std::string_view& GetDesc();

		const std::string_view& GetText();

		ImVec2 GetOptionSize(const UIConfiguration& Cfg);/*TextSize INCLUDED*/
		ImVec2 GetOptionTextSize(const UIConfiguration& Cfg);

		void UpdateInput(UINT Msg, std::uint32_t Key, LPARAM lParam);

	private:

		std::string_view m_Text;
		std::shared_ptr<OptionParams> m_Params;

		std::function<void(OptionParams*)> m_Callback;

		bool m_CaptureInputs;
	};
}

