#pragma once

enum class AnimationMode : std::uint8_t
{
    Classic = 0,
    Arc = 1,
    Bounce = 2,
    Spiral = 3,
    Fountain = 4,
    Matrix = 5
};

struct ColorRGB
{
    float r{ 1.0f };
    float g{ 1.0f };
    float b{ 1.0f };
};

class Settings
{
public:
    static Settings* GetSingleton()
    {
        static Settings singleton;
        return &singleton;
    }

    void Load();
    void Save() const;

    bool enabled{ true };
    bool showHealing{ true };
    bool showDamage{ true };
    bool showPlusForHealing{ true };
    float duration{ 1.5f };
    float baseFontSize{ 22.0f };
    float worldOffsetZ{ 70.0f };
    float worldOffsetXY{ 6.0f };
    float screenOffsetX{ 0.0f };
    float screenOffsetY{ 0.0f };
    float maxDistance{ 4000.0f };
    bool scaleWithDistance{ true };
    float distanceScaleMin{ 0.75f };
    float distanceScaleMax{ 1.25f };

    ColorRGB damageColor{ 1.0f, 0.4f, 0.4f };
    ColorRGB healColor{ 1.0f, 1.0f, 1.0f };

    AnimationMode animationMode{ AnimationMode::Arc };
    float classicRiseSpeed{ 150.0f };
    float classicFadeStart{ 0.3f };
    float arcHeight{ 200.0f };
    float arcSpread{ 100.0f };
    float arcGravity{ 300.0f };
    float bounceInitialVelocity{ 250.0f };
    float bounceGravity{ 500.0f };
    float bounceDamping{ 0.7f };
    float bounceGroundLevel{ 50.0f };
    float spiralRadius{ 60.0f };
    float spiralSpeed{ 2.0f };
    float spiralRiseSpeed{ 80.0f };
    float spiralExpand{ 2.0f };
    float fountainInitialVelocity{ 150.0f };
    float fountainGravity{ 400.0f };
    float fountainSpread{ 120.0f };
    float fountainRandomness{ 0.3f };
    float matrixFallSpeed{ 200.0f };
    float matrixFadeDelay{ 0.2f };
    float matrixTravelDistance{ 150.0f };
    bool matrixGreenTint{ true };

private:
    static constexpr auto kIniPath = L"Data/SKSE/Plugins/FloatingDamageSKSE.ini";
    static constexpr auto kIniPathA = "Data/SKSE/Plugins/FloatingDamageSKSE.ini";

    Settings() = default;
};

