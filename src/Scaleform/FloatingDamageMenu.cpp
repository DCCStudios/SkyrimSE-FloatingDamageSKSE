#include "Scaleform/FloatingDamageMenu.h"

#include "FloatingDamageManager.h"

namespace Scaleform
{
    FloatingDamageMenu::FloatingDamageMenu()
    {
        auto menu = static_cast<Super*>(this);
        menu->depthPriority = SortPriority();

        auto scaleformManager = RE::BSScaleformManager::GetSingleton();
        if (scaleformManager) {
            [[maybe_unused]] const auto success =
                scaleformManager->LoadMovieEx(menu, FILE_NAME, [](RE::GFxMovieDef*) -> void {});
        }

        menuFlags.set(RE::UI_MENU_FLAGS::kAlwaysOpen);
        menuFlags.set(RE::UI_MENU_FLAGS::kRequiresUpdate);
        menuFlags.set(RE::UI_MENU_FLAGS::kAllowSaving);
        menu->inputContext = Context::kNone;

        _view = menu->uiMovie;
        if (_view) {
            _view->SetMouseCursorCount(0);
        }
    }

    void FloatingDamageMenu::Register()
    {
        auto ui = RE::UI::GetSingleton();
        if (ui) {
            ui->Register(MENU_NAME, Creator);
            logger::info("Registered {}"sv, MENU_NAME);
        }
    }

    void FloatingDamageMenu::PostCreate()
    {
        OnOpen();
        Super::PostCreate();
    }

    FloatingDamageMenu::UIResult FloatingDamageMenu::ProcessMessage(RE::UIMessage& a_message)
    {
        using Type = RE::UI_MESSAGE_TYPE;

        switch (*a_message.type) {
        case Type::kShow:
            OnOpen();
            return Super::ProcessMessage(a_message);
        case Type::kHide:
            OnClose();
            return Super::ProcessMessage(a_message);
        default:
            return Super::ProcessMessage(a_message);
        }
    }

    void FloatingDamageMenu::AdvanceMovie(float, std::uint32_t)
    {
        auto currentTime = chrono_clock::now();
        auto dt = (currentTime - _movieLastTime) / 1000000000.f;
        _movieLastTime = currentTime;

        float deltaTime = static_cast<float>(dt.count());
        Update(deltaTime);

        if (uiMovie) {
            uiMovie->Advance(deltaTime);
        }
    }

    void FloatingDamageMenu::OnOpen()
    {
        _isOpen = true;
        if (_view) {
            RE::GFxValue args[1];
            args[0].SetNumber(2);  // visible
            _view->Invoke("_root.TrueHUD.SetVisibilityMode", nullptr, args, 1);
        }
    }

    void FloatingDamageMenu::OnClose()
    {
        _isOpen = false;
    }

    void FloatingDamageMenu::Update(float)
    {
        if (!_view) {
            return;
        }

        FloatingDamageManager::GetSingleton()->UpdateScaleform(_view);
    }
}

