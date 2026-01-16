#include "Scaleform/FloatingDamageMenu.h"

#include "FloatingDamageManager.h"
#include "Settings.h"

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
        _configured = false;
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

        if (!_configured) {
            auto* settings = Settings::GetSingleton();
            RE::GFxValue args[28];
            args[0].SetString("$EverywhereMediumFont");
            args[1].SetNumber(0);
            args[2].SetNumber(0);
            args[3].SetNumber(settings->classicRiseSpeed);
            args[4].SetNumber(settings->classicFadeStart);
            args[5].SetNumber(settings->arcHeight);
            args[6].SetNumber(settings->arcSpread);
            args[7].SetNumber(settings->arcGravity);
            args[8].SetNumber(settings->bounceInitialVelocity);
            args[9].SetNumber(settings->bounceGravity);
            args[10].SetNumber(settings->bounceDamping);
            args[11].SetNumber(settings->bounceGroundLevel);
            args[12].SetNumber(settings->spiralRadius);
            args[13].SetNumber(settings->spiralSpeed);
            args[14].SetNumber(settings->spiralRiseSpeed);
            args[15].SetNumber(settings->spiralExpand);
            args[16].SetNumber(settings->fountainInitialVelocity);
            args[17].SetNumber(settings->fountainGravity);
            args[18].SetNumber(settings->fountainSpread);
            args[19].SetNumber(settings->fountainRandomness);
            args[20].SetNumber(settings->matrixFallSpeed);
            args[21].SetNumber(settings->matrixTravelDistance);
            args[22].SetNumber(settings->matrixFadeDelay);
            args[23].SetNumber(settings->distanceScaleMin);
            args[24].SetNumber(settings->distanceScaleMax);
            args[25].SetNumber(settings->baseFontSize);
            args[26].SetNumber(settings->screenOffsetX);
            args[27].SetNumber(settings->screenOffsetY);

            _view->Invoke("_root.widget.Setup", nullptr, args, 28);
            _configured = true;
        }

        FloatingDamageManager::GetSingleton()->UpdateScaleform(_view);
    }
}

