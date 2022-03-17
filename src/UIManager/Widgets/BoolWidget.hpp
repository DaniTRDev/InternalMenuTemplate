#pragma once

namespace change_me
{
    class BoolWidget : public WidgetBase
    {
    public:

        BoolWidget(bool * Bool);

        ImVec2 GetWidgetSize(float WrapWidth) override;
        void DrawWidget(ImVec2 Pos, class UIConfiguration Cfg) override;

        void UpdateInputs(bool& WantCapture, UINT Msg, std::uint32_t Key, LPARAM lParam) override;

    private:

        ImVec2 m_CircleSize { 5.f, 5.f };

        bool* m_Bool;
    };
}

