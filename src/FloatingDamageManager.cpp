#include "FloatingDamageManager.h"
#include "Settings.h"

FloatingDamageManager::FloatingDamageManager()
{
    std::random_device rd;
    _rng.seed(rd());
    _startTime = std::chrono::steady_clock::now();
}

float FloatingDamageManager::NowSeconds() const
{
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<float>(now - _startTime).count();
}

void FloatingDamageManager::AddFloatingNumber(RE::Actor* a_actor, float a_delta, bool a_isHeal)
{
    if (!a_actor) {
        return;
    }

    auto* settings = Settings::GetSingleton();
    if (!settings->enabled) {
        return;
    }

    if (a_isHeal && !settings->showHealing) {
        return;
    }

    if (!a_isHeal && !settings->showDamage) {
        return;
    }

    if (a_delta == 0.0f) {
        return;
    }

    if (a_actor->IsPlayerRef()) {
        return;
    }

    if (!a_actor->Is3DLoaded()) {
        return;
    }

    std::uniform_real_distribution<float> randOffset(-settings->worldOffsetXY, settings->worldOffsetXY);

    Entry entry;
    entry.origin = a_actor->GetPosition();
    entry.origin.x += randOffset(_rng);
    entry.origin.y += randOffset(_rng);
    entry.origin.z += settings->worldOffsetZ;
    entry.startTime = NowSeconds();
    entry.duration = settings->duration;
    entry.magnitude = std::abs(a_delta);
    entry.isHeal = a_isHeal;
    entry.mode = settings->animationMode;
    entry.text = FormatNumber(entry.magnitude, entry.isHeal);
    entry.formId = a_actor->GetFormID();

    if (entry.mode == AnimationMode::Matrix && settings->matrixGreenTint && !entry.isHeal) {
        entry.color = 0x00FF00;
    } else {
        entry.color = entry.isHeal ? MakeColor(settings->healColor) : MakeColor(settings->damageColor);
    }

    _entries.push_back(entry);
}

void FloatingDamageManager::UpdateScaleform(RE::GPtr<RE::GFxMovieView> a_view)
{
    if (!a_view) {
        return;
    }

    auto* settings = Settings::GetSingleton();
    if (!settings->enabled) {
        return;
    }

    auto* player = RE::PlayerCharacter::GetSingleton();
    if (!player) {
        return;
    }

    for (auto it = _entries.begin(); it != _entries.end();) {
        float age = NowSeconds() - it->startTime;
        if (age < 0.0f) {
            ++it;
            continue;
        }

        RE::GFxValue textArray;
        a_view->CreateArray(&textArray);
        RE::GFxValue textValue;
        textValue.SetString(it->text);
        textArray.PushBack(textValue);

        RE::GFxValue sizeArray;
        a_view->CreateArray(&sizeArray);
        RE::GFxValue sizeValue;
        sizeValue.SetNumber(settings->baseFontSize);
        sizeArray.PushBack(sizeValue);

        RE::GFxValue colorArray;
        a_view->CreateArray(&colorArray);
        RE::GFxValue colorValue;
        colorValue.SetNumber(it->color);
        colorArray.PushBack(colorValue);

        RE::GFxValue args[11];
        args[0] = textArray;
        args[1] = sizeArray;
        args[2] = colorArray;
        args[3].SetNumber(it->origin.x);
        args[4].SetNumber(it->origin.y);
        args[5].SetNumber(it->origin.z);
        args[6].SetNumber(static_cast<double>(it->formId));
        args[7].SetNumber(100.0);
        args[8].SetBoolean(!it->isHeal);
        args[9].SetNumber(static_cast<int>(it->mode));
        args[10].SetNumber(it->duration);

        a_view->Invoke("_root.widget.PopupTextTrackEx", nullptr, args, 11);

        it = _entries.erase(it);
    }
}

void FloatingDamageManager::Clear()
{
    _entries.clear();
}

std::string FloatingDamageManager::FormatNumber(float a_value, bool a_isHeal) const
{
    int rounded = static_cast<int>(std::round(a_value));
    if (a_isHeal && Settings::GetSingleton()->showPlusForHealing) {
        return std::format("+{}", rounded);
    }
    return std::format("{}", rounded);
}

uint32_t FloatingDamageManager::MakeColor(const ColorRGB& a_color) const
{
    auto r = static_cast<uint32_t>(std::clamp(a_color.r, 0.0f, 1.0f) * 255.0f);
    auto g = static_cast<uint32_t>(std::clamp(a_color.g, 0.0f, 1.0f) * 255.0f);
    auto b = static_cast<uint32_t>(std::clamp(a_color.b, 0.0f, 1.0f) * 255.0f);
    return (r << 16) | (g << 8) | b;
}

