#pragma once

#include "Settings.h"

class FloatingDamageManager
{
public:
    static FloatingDamageManager* GetSingleton()
    {
        static FloatingDamageManager singleton;
        return &singleton;
    }

    void AddFloatingNumber(RE::Actor* a_actor, float a_delta, bool a_isHeal);
    void UpdateScaleform(RE::GPtr<RE::GFxMovieView> a_view);
    void Clear();

private:
    struct Entry
    {
        std::string text;
        uint32_t color{ 0xFFFFFF };
        RE::NiPoint3 origin{};
        float startTime{ 0.0f };
        float duration{ 1.0f };
        float magnitude{ 0.0f };
        bool isHeal{ false };
        AnimationMode mode{ AnimationMode::Classic };
        RE::FormID formId{ 0 };
    };

    FloatingDamageManager();

    float NowSeconds() const;
    std::string FormatNumber(float a_value, bool a_isHeal) const;
    uint32_t MakeColor(const ColorRGB& a_color) const;

    std::vector<Entry> _entries;
    std::mt19937 _rng;
    std::chrono::steady_clock::time_point _startTime;
};

