#include "Settings.h"

namespace
{
    float ClampFloat(float value, float minValue, float maxValue)
    {
        return std::clamp(value, minValue, maxValue);
    }

    int ClampInt(int value, int minValue, int maxValue)
    {
        return std::clamp(value, minValue, maxValue);
    }

    ColorRGB ReadColor(CSimpleIniA& ini, const char* section, const char* rKey, const char* gKey, const char* bKey, int defR, int defG, int defB)
    {
        int r = ClampInt(static_cast<int>(ini.GetLongValue(section, rKey, defR)), 0, 255);
        int g = ClampInt(static_cast<int>(ini.GetLongValue(section, gKey, defG)), 0, 255);
        int b = ClampInt(static_cast<int>(ini.GetLongValue(section, bKey, defB)), 0, 255);
        return {
            static_cast<float>(r) / 255.0f,
            static_cast<float>(g) / 255.0f,
            static_cast<float>(b) / 255.0f
        };
    }
}

void Settings::Load()
{
    CSimpleIniA ini;
    ini.SetUnicode();

    if (ini.LoadFile(kIniPath) == SI_OK) {
        enabled = ini.GetBoolValue("General", "bEnabled", enabled);
        showHealing = ini.GetBoolValue("General", "bShowHealing", showHealing);
        showDamage = ini.GetBoolValue("General", "bShowDamage", showDamage);
        showPlusForHealing = ini.GetBoolValue("General", "bShowPlusForHealing", showPlusForHealing);
        duration = ClampFloat(static_cast<float>(ini.GetDoubleValue("General", "fDuration", duration)), 0.2f, 10.0f);
        baseFontSize = ClampFloat(static_cast<float>(ini.GetDoubleValue("General", "fBaseFontSize", baseFontSize)), 8.0f, 128.0f);
        worldOffsetZ = ClampFloat(static_cast<float>(ini.GetDoubleValue("General", "fWorldOffsetZ", worldOffsetZ)), 0.0f, 200.0f);
        worldOffsetXY = ClampFloat(static_cast<float>(ini.GetDoubleValue("General", "fWorldOffsetXY", worldOffsetXY)), 0.0f, 50.0f);
        screenOffsetX = ClampFloat(static_cast<float>(ini.GetDoubleValue("General", "fScreenOffsetX", screenOffsetX)), -0.5f, 0.5f);
        screenOffsetY = ClampFloat(static_cast<float>(ini.GetDoubleValue("General", "fScreenOffsetY", screenOffsetY)), -0.5f, 0.5f);
        maxDistance = ClampFloat(static_cast<float>(ini.GetDoubleValue("General", "fMaxDistance", maxDistance)), 100.0f, 15000.0f);
        scaleWithDistance = ini.GetBoolValue("General", "bScaleWithDistance", scaleWithDistance);
        distanceScaleMin = ClampFloat(static_cast<float>(ini.GetDoubleValue("General", "fScaleMin", distanceScaleMin)), 0.25f, 1.0f);
        distanceScaleMax = ClampFloat(static_cast<float>(ini.GetDoubleValue("General", "fScaleMax", distanceScaleMax)), 1.0f, 2.0f);

        damageColor = ReadColor(ini, "Colors", "iDamageR", "iDamageG", "iDamageB", 255, 102, 102);
        healColor = ReadColor(ini, "Colors", "iHealR", "iHealG", "iHealB", 255, 255, 255);

        animationMode = static_cast<AnimationMode>(ClampInt(static_cast<int>(ini.GetLongValue("Animation", "iMode", static_cast<int>(animationMode))), 0, 5));
        classicRiseSpeed = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fClassicRiseSpeed", classicRiseSpeed)), 10.0f, 600.0f);
        classicFadeStart = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fClassicFadeStart", classicFadeStart)), 0.0f, 1.0f);
        arcHeight = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fArcHeight", arcHeight)), 10.0f, 800.0f);
        arcSpread = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fArcSpread", arcSpread)), 0.0f, 400.0f);
        arcGravity = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fArcGravity", arcGravity)), 10.0f, 1200.0f);
        bounceInitialVelocity = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fBounceInitialVelocity", bounceInitialVelocity)), 10.0f, 800.0f);
        bounceGravity = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fBounceGravity", bounceGravity)), 10.0f, 1200.0f);
        bounceDamping = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fBounceDamping", bounceDamping)), 0.1f, 1.0f);
        bounceGroundLevel = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fBounceGroundLevel", bounceGroundLevel)), 0.0f, 200.0f);
        spiralRadius = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fSpiralRadius", spiralRadius)), 0.0f, 200.0f);
        spiralSpeed = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fSpiralSpeed", spiralSpeed)), 0.1f, 10.0f);
        spiralRiseSpeed = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fSpiralRiseSpeed", spiralRiseSpeed)), 10.0f, 300.0f);
        spiralExpand = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fSpiralExpand", spiralExpand)), 0.0f, 5.0f);
        fountainInitialVelocity = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fFountainInitialVelocity", fountainInitialVelocity)), 10.0f, 400.0f);
        fountainGravity = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fFountainGravity", fountainGravity)), 10.0f, 1200.0f);
        fountainSpread = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fFountainSpread", fountainSpread)), 0.0f, 300.0f);
        fountainRandomness = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fFountainRandomness", fountainRandomness)), 0.0f, 1.0f);
        matrixFallSpeed = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fMatrixFallSpeed", matrixFallSpeed)), 10.0f, 600.0f);
        matrixFadeDelay = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fMatrixFadeDelay", matrixFadeDelay)), 0.0f, 1.0f);
        matrixTravelDistance = ClampFloat(static_cast<float>(ini.GetDoubleValue("Animation", "fMatrixTravelDistance", matrixTravelDistance)), 10.0f, 400.0f);
        matrixGreenTint = ini.GetBoolValue("Animation", "bMatrixGreenTint", matrixGreenTint);
    } else {
        Save();
    }
}

void Settings::Save() const
{
    CSimpleIniA ini;
    ini.SetUnicode();

    ini.SetBoolValue("General", "bEnabled", enabled, "; Master enable/disable");
    ini.SetBoolValue("General", "bShowHealing", showHealing, "; Show healing numbers");
    ini.SetBoolValue("General", "bShowDamage", showDamage, "; Show damage numbers");
    ini.SetBoolValue("General", "bShowPlusForHealing", showPlusForHealing, "; Prefix healing with +");
    ini.SetDoubleValue("General", "fDuration", duration, "; Lifetime in seconds");
    ini.SetDoubleValue("General", "fBaseFontSize", baseFontSize, "; Base font size in pixels");
    ini.SetDoubleValue("General", "fWorldOffsetZ", worldOffsetZ, "; Vertical offset from actor position");
    ini.SetDoubleValue("General", "fWorldOffsetXY", worldOffsetXY, "; Random XY offset range");
    ini.SetDoubleValue("General", "fScreenOffsetX", screenOffsetX, "; Screen-space X offset (-0.5 to 0.5)");
    ini.SetDoubleValue("General", "fScreenOffsetY", screenOffsetY, "; Screen-space Y offset (-0.5 to 0.5)");
    ini.SetDoubleValue("General", "fMaxDistance", maxDistance, "; Max display distance");
    ini.SetBoolValue("General", "bScaleWithDistance", scaleWithDistance, "; Scale text with distance");
    ini.SetDoubleValue("General", "fScaleMin", distanceScaleMin, "; Minimum distance scale");
    ini.SetDoubleValue("General", "fScaleMax", distanceScaleMax, "; Maximum distance scale");

    ini.SetLongValue("Colors", "iDamageR", static_cast<long>(damageColor.r * 255.0f), "; Damage color (RGB 0-255)");
    ini.SetLongValue("Colors", "iDamageG", static_cast<long>(damageColor.g * 255.0f));
    ini.SetLongValue("Colors", "iDamageB", static_cast<long>(damageColor.b * 255.0f));
    ini.SetLongValue("Colors", "iHealR", static_cast<long>(healColor.r * 255.0f), "; Healing color (RGB 0-255)");
    ini.SetLongValue("Colors", "iHealG", static_cast<long>(healColor.g * 255.0f));
    ini.SetLongValue("Colors", "iHealB", static_cast<long>(healColor.b * 255.0f));

    ini.SetLongValue("Animation", "iMode", static_cast<long>(animationMode), "; 0=Classic,1=Arc,2=Bounce,3=Spiral,4=Fountain,5=Matrix");
    ini.SetDoubleValue("Animation", "fClassicRiseSpeed", classicRiseSpeed);
    ini.SetDoubleValue("Animation", "fClassicFadeStart", classicFadeStart);
    ini.SetDoubleValue("Animation", "fArcHeight", arcHeight);
    ini.SetDoubleValue("Animation", "fArcSpread", arcSpread);
    ini.SetDoubleValue("Animation", "fArcGravity", arcGravity);
    ini.SetDoubleValue("Animation", "fBounceInitialVelocity", bounceInitialVelocity);
    ini.SetDoubleValue("Animation", "fBounceGravity", bounceGravity);
    ini.SetDoubleValue("Animation", "fBounceDamping", bounceDamping);
    ini.SetDoubleValue("Animation", "fBounceGroundLevel", bounceGroundLevel);
    ini.SetDoubleValue("Animation", "fSpiralRadius", spiralRadius);
    ini.SetDoubleValue("Animation", "fSpiralSpeed", spiralSpeed);
    ini.SetDoubleValue("Animation", "fSpiralRiseSpeed", spiralRiseSpeed);
    ini.SetDoubleValue("Animation", "fSpiralExpand", spiralExpand);
    ini.SetDoubleValue("Animation", "fFountainInitialVelocity", fountainInitialVelocity);
    ini.SetDoubleValue("Animation", "fFountainGravity", fountainGravity);
    ini.SetDoubleValue("Animation", "fFountainSpread", fountainSpread);
    ini.SetDoubleValue("Animation", "fFountainRandomness", fountainRandomness);
    ini.SetDoubleValue("Animation", "fMatrixFallSpeed", matrixFallSpeed);
    ini.SetDoubleValue("Animation", "fMatrixFadeDelay", matrixFadeDelay);
    ini.SetDoubleValue("Animation", "fMatrixTravelDistance", matrixTravelDistance);
    ini.SetBoolValue("Animation", "bMatrixGreenTint", matrixGreenTint);

    ini.SaveFile(kIniPathA);
}

