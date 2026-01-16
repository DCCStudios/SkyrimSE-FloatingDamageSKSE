#pragma once

namespace Scaleform
{
    class FloatingDamageMenu : public RE::IMenu
    {
    private:
        using Super = RE::IMenu;
        using UIResult = RE::UI_MESSAGE_RESULTS;
        using chrono_clock = std::chrono::steady_clock;

    public:
        static constexpr std::string_view MenuName() noexcept { return MENU_NAME; }
        static constexpr std::int8_t SortPriority() noexcept { return SORT_PRIORITY; }

        static void Register();

        FloatingDamageMenu();
        ~FloatingDamageMenu() override = default;

        static RE::stl::owner<RE::IMenu*> Creator() { return new FloatingDamageMenu(); }

        void PostCreate() override;
        UIResult ProcessMessage(RE::UIMessage& a_message) override;
        void AdvanceMovie(float, std::uint32_t) override;

    private:
        void OnOpen();
        void OnClose();
        void Update(float a_deltaTime);

        RE::GPtr<RE::GFxMovieView> _view;
        bool _isOpen{ false };
        chrono_clock::time_point _movieLastTime{ chrono_clock::now() };

        static constexpr std::string_view FILE_NAME{ "TrueHUD" };
        static constexpr std::string_view MENU_NAME{ "FloatingDamageSKSE" };
        static constexpr std::int8_t SORT_PRIORITY{ 0 };
    };
}

