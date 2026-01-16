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
        RE::GFxValue object;
        uint32_t widgetID{ 0 };
        bool created{ false };
        RE::NiPoint3 origin{};
        float startTime{ 0.0f };
        float lastUpdateTime{ 0.0f };
        float duration{ 1.0f };
        float magnitude{ 0.0f };
        bool isHeal{ false };
        AnimationMode mode{ AnimationMode::Classic };
        float arcAngle{ 0.0f };
        float spiralAngle{ 0.0f };
        float matrixDirection{ 1.0f };
        RE::NiPoint3 offset{};
        RE::NiPoint3 velocity{};
    };

    FloatingDamageManager();

    float NowSeconds() const;
    bool WorldToScreen(const RE::NiPoint3& a_world, float a_screenWidth, float a_screenHeight, float& a_x, float& a_y, float& a_depth) const;
    RE::NiPoint3 ComputeOffset(Entry& a_entry, float a_age, float a_delta);
    float ComputeAlpha(const Entry& a_entry, float a_progress) const;
    float ComputeDistanceScale(float a_distance) const;
    std::string FormatNumber(float a_value, bool a_isHeal) const;
    uint32_t MakeColor(const ColorRGB& a_color) const;

    std::vector<Entry> _entries;
    std::mt19937 _rng;
    std::chrono::steady_clock::time_point _startTime;
    uint32_t _nextWidgetID{ 0 };
};

