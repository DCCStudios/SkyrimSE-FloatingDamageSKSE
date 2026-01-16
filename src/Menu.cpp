#include "Menu.h"
#include "FloatingDamageManager.h"
#include "Settings.h"
#include "SKSEMenuFramework.h"

namespace Menu
{
    namespace State
    {
        inline bool initialized{ false };
        inline bool hasUnsavedChanges{ false };
    }

    void Register()
    {
        if (!SKSEMenuFramework::IsInstalled()) {
            logger::warn("SKSE Menu Framework not installed; settings menu disabled");
            return;
        }

        HMODULE frameworkHandle = GetModuleHandleW(L"SKSEMenuFramework");
        if (!frameworkHandle) {
            logger::warn("SKSE Menu Framework DLL found but not loaded; menu disabled");
            return;
        }

        SKSEMenuFramework::SetSection("Floating Damage SKSE");
        SKSEMenuFramework::AddSectionItem("Settings", RenderSettings);
        logger::info("Registered Floating Damage SKSE menu");
    }

    void DrawGeneralSettings()
    {
        auto* settings = Settings::GetSingleton();

        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "General");
        ImGui::Separator();

        bool enabled = settings->enabled;
        if (ImGui::Checkbox("Enable Floating Damage Numbers", &enabled)) {
            settings->enabled = enabled;
            State::hasUnsavedChanges = true;
        }

        bool showDamage = settings->showDamage;
        if (ImGui::Checkbox("Show Damage", &showDamage)) {
            settings->showDamage = showDamage;
            State::hasUnsavedChanges = true;
        }

        bool showHealing = settings->showHealing;
        if (ImGui::Checkbox("Show Healing", &showHealing)) {
            settings->showHealing = showHealing;
            State::hasUnsavedChanges = true;
        }

        bool showPlus = settings->showPlusForHealing;
        if (ImGui::Checkbox("Show + for Healing", &showPlus)) {
            settings->showPlusForHealing = showPlus;
            State::hasUnsavedChanges = true;
        }

        float duration = settings->duration;
        if (ImGui::SliderFloat("Duration (sec)", &duration, 0.2f, 5.0f, "%.2f")) {
            settings->duration = duration;
            State::hasUnsavedChanges = true;
        }

        float fontSize = settings->baseFontSize;
        if (ImGui::SliderFloat("Base Font Size", &fontSize, 10.0f, 64.0f, "%.0f")) {
            settings->baseFontSize = fontSize;
            State::hasUnsavedChanges = true;
        }

        float maxDistance = settings->maxDistance;
        if (ImGui::SliderFloat("Max Distance", &maxDistance, 100.0f, 15000.0f, "%.0f")) {
            settings->maxDistance = maxDistance;
            State::hasUnsavedChanges = true;
        }

        float offsetZ = settings->worldOffsetZ;
        if (ImGui::SliderFloat("World Offset Z", &offsetZ, 0.0f, 200.0f, "%.1f")) {
            settings->worldOffsetZ = offsetZ;
            State::hasUnsavedChanges = true;
        }

        float offsetXY = settings->worldOffsetXY;
        if (ImGui::SliderFloat("Random XY Spread", &offsetXY, 0.0f, 30.0f, "%.1f")) {
            settings->worldOffsetXY = offsetXY;
            State::hasUnsavedChanges = true;
        }

        float screenOffsetX = settings->screenOffsetX;
        if (ImGui::SliderFloat("Screen Offset X", &screenOffsetX, -0.3f, 0.3f, "%.2f")) {
            settings->screenOffsetX = screenOffsetX;
            State::hasUnsavedChanges = true;
        }

        float screenOffsetY = settings->screenOffsetY;
        if (ImGui::SliderFloat("Screen Offset Y", &screenOffsetY, -0.3f, 0.3f, "%.2f")) {
            settings->screenOffsetY = screenOffsetY;
            State::hasUnsavedChanges = true;
        }
    }

    void DrawColorSettings()
    {
        auto* settings = Settings::GetSingleton();

        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Colors");
        ImGui::Separator();

        float damageColor[3] = { settings->damageColor.r, settings->damageColor.g, settings->damageColor.b };
        if (ImGui::ColorEdit3("Damage Color", damageColor)) {
            settings->damageColor = { damageColor[0], damageColor[1], damageColor[2] };
            State::hasUnsavedChanges = true;
        }

        float healColor[3] = { settings->healColor.r, settings->healColor.g, settings->healColor.b };
        if (ImGui::ColorEdit3("Healing Color", healColor)) {
            settings->healColor = { healColor[0], healColor[1], healColor[2] };
            State::hasUnsavedChanges = true;
        }

        bool matrixTint = settings->matrixGreenTint;
        if (ImGui::Checkbox("Matrix Mode Green Tint", &matrixTint)) {
            settings->matrixGreenTint = matrixTint;
            State::hasUnsavedChanges = true;
        }
    }

    void DrawAnimationSettings()
    {
        auto* settings = Settings::GetSingleton();

        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Animation");
        ImGui::Separator();

        const char* modeNames[] = { "Classic", "Arc", "Bounce", "Spiral", "Fountain", "Matrix" };
        int currentMode = static_cast<int>(settings->animationMode);
        if (ImGui::Combo("Mode", &currentMode, modeNames, static_cast<int>(std::size(modeNames)))) {
            settings->animationMode = static_cast<AnimationMode>(currentMode);
            State::hasUnsavedChanges = true;
        }

        if (ImGui::CollapsingHeader("Classic", ImGuiTreeNodeFlags_DefaultOpen)) {
            float rise = settings->classicRiseSpeed;
            if (ImGui::SliderFloat("Rise Speed", &rise, 10.0f, 400.0f, "%.1f")) {
                settings->classicRiseSpeed = rise;
                State::hasUnsavedChanges = true;
            }

            float fadeStart = settings->classicFadeStart;
            if (ImGui::SliderFloat("Fade Start", &fadeStart, 0.0f, 1.0f, "%.2f")) {
                settings->classicFadeStart = fadeStart;
                State::hasUnsavedChanges = true;
            }
        }

        if (ImGui::CollapsingHeader("Arc")) {
            float height = settings->arcHeight;
            if (ImGui::SliderFloat("Height", &height, 10.0f, 500.0f, "%.1f")) {
                settings->arcHeight = height;
                State::hasUnsavedChanges = true;
            }

            float spread = settings->arcSpread;
            if (ImGui::SliderFloat("Spread", &spread, 0.0f, 300.0f, "%.1f")) {
                settings->arcSpread = spread;
                State::hasUnsavedChanges = true;
            }

            float gravity = settings->arcGravity;
            if (ImGui::SliderFloat("Gravity", &gravity, 10.0f, 800.0f, "%.1f")) {
                settings->arcGravity = gravity;
                State::hasUnsavedChanges = true;
            }
        }

        if (ImGui::CollapsingHeader("Bounce")) {
            float initial = settings->bounceInitialVelocity;
            if (ImGui::SliderFloat("Initial Velocity", &initial, 10.0f, 500.0f, "%.1f")) {
                settings->bounceInitialVelocity = initial;
                State::hasUnsavedChanges = true;
            }

            float gravity = settings->bounceGravity;
            if (ImGui::SliderFloat("Gravity##Bounce", &gravity, 10.0f, 900.0f, "%.1f")) {
                settings->bounceGravity = gravity;
                State::hasUnsavedChanges = true;
            }

            float damping = settings->bounceDamping;
            if (ImGui::SliderFloat("Damping", &damping, 0.1f, 1.0f, "%.2f")) {
                settings->bounceDamping = damping;
                State::hasUnsavedChanges = true;
            }

            float ground = settings->bounceGroundLevel;
            if (ImGui::SliderFloat("Ground Level", &ground, 0.0f, 200.0f, "%.1f")) {
                settings->bounceGroundLevel = ground;
                State::hasUnsavedChanges = true;
            }
        }

        if (ImGui::CollapsingHeader("Spiral")) {
            float radius = settings->spiralRadius;
            if (ImGui::SliderFloat("Radius", &radius, 0.0f, 150.0f, "%.1f")) {
                settings->spiralRadius = radius;
                State::hasUnsavedChanges = true;
            }

            float speed = settings->spiralSpeed;
            if (ImGui::SliderFloat("Speed", &speed, 0.1f, 8.0f, "%.2f")) {
                settings->spiralSpeed = speed;
                State::hasUnsavedChanges = true;
            }

            float rise = settings->spiralRiseSpeed;
            if (ImGui::SliderFloat("Rise Speed##Spiral", &rise, 10.0f, 200.0f, "%.1f")) {
                settings->spiralRiseSpeed = rise;
                State::hasUnsavedChanges = true;
            }

            float expand = settings->spiralExpand;
            if (ImGui::SliderFloat("Expand", &expand, 0.0f, 3.0f, "%.2f")) {
                settings->spiralExpand = expand;
                State::hasUnsavedChanges = true;
            }
        }

        if (ImGui::CollapsingHeader("Fountain")) {
            float initVel = settings->fountainInitialVelocity;
            if (ImGui::SliderFloat("Initial Velocity##Fountain", &initVel, 10.0f, 300.0f, "%.1f")) {
                settings->fountainInitialVelocity = initVel;
                State::hasUnsavedChanges = true;
            }

            float gravity = settings->fountainGravity;
            if (ImGui::SliderFloat("Gravity##Fountain", &gravity, 10.0f, 900.0f, "%.1f")) {
                settings->fountainGravity = gravity;
                State::hasUnsavedChanges = true;
            }

            float spread = settings->fountainSpread;
            if (ImGui::SliderFloat("Spread##Fountain", &spread, 0.0f, 200.0f, "%.1f")) {
                settings->fountainSpread = spread;
                State::hasUnsavedChanges = true;
            }

            float randomness = settings->fountainRandomness;
            if (ImGui::SliderFloat("Randomness", &randomness, 0.0f, 1.0f, "%.2f")) {
                settings->fountainRandomness = randomness;
                State::hasUnsavedChanges = true;
            }
        }

        if (ImGui::CollapsingHeader("Matrix")) {
            float fallSpeed = settings->matrixFallSpeed;
            if (ImGui::SliderFloat("Fall Speed", &fallSpeed, 10.0f, 400.0f, "%.1f")) {
                settings->matrixFallSpeed = fallSpeed;
                State::hasUnsavedChanges = true;
            }

            float fadeDelay = settings->matrixFadeDelay;
            if (ImGui::SliderFloat("Fade Delay", &fadeDelay, 0.0f, 1.0f, "%.2f")) {
                settings->matrixFadeDelay = fadeDelay;
                State::hasUnsavedChanges = true;
            }

            float travel = settings->matrixTravelDistance;
            if (ImGui::SliderFloat("Travel Distance", &travel, 10.0f, 250.0f, "%.1f")) {
                settings->matrixTravelDistance = travel;
                State::hasUnsavedChanges = true;
            }
        }
    }

    void __stdcall RenderSettings()
    {
        auto* settings = Settings::GetSingleton();

        if (!State::initialized) {
            settings->Load();
            State::initialized = true;
        }

        if (ImGui::BeginTabBar("FloatingDamageTabs")) {
            if (ImGui::BeginTabItem("General")) {
                DrawGeneralSettings();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Colors")) {
                DrawColorSettings();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Animation")) {
                DrawAnimationSettings();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::Spacing();
        ImGui::Separator();

        if (State::hasUnsavedChanges) {
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "Unsaved changes");
        }

        if (ImGui::Button("Save Settings", ImVec2(140, 0))) {
            settings->Save();
            State::hasUnsavedChanges = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Reload Settings", ImVec2(140, 0))) {
            settings->Load();
            State::hasUnsavedChanges = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Active Numbers", ImVec2(160, 0))) {
            FloatingDamageManager::GetSingleton()->Clear();
        }
    }

}

