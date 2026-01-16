#pragma once

#include "Settings.h"

struct FloatingDrawItem
{
    std::string text;
    float x{ 0.0f };
    float y{ 0.0f };
    float fontSize{ 22.0f };
    ColorRGB color;
    float alpha{ 1.0f };
};

class FloatingDamageManager
{
public:
    static FloatingDamageManager* GetSingleton()
    {
        static FloatingDamageManager singleton;
        return &singleton;
    }

    void AddFloatingNumber(RE::Actor* a_actor, float a_delta, bool a_isHeal);
    void CollectDrawItems(std::vector<FloatingDrawItem>& a_items, float a_screenWidth, float a_screenHeight);
    void Clear();

private:
    struct Entry
    {
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

    std::vector<Entry> _entries;
    std::mt19937 _rng;
    std::chrono::steady_clock::time_point _startTime;
};

