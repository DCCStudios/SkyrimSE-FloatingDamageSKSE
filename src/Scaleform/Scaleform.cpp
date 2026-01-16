#include "Scaleform/Scaleform.h"

#include "Offsets.h"
#include "Scaleform/FloatingDamageMenu.h"

namespace Scaleform
{
    class WorldPtToScreenPt3Handler : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            if (!a_params.retVal || a_params.argCount < 4) {
                return;
            }

            float x = static_cast<float>(a_params.args[0].GetNumber());
            float y = static_cast<float>(a_params.args[1].GetNumber());
            float z = static_cast<float>(a_params.args[2].GetNumber());

            RE::NiPoint3 worldPos{ x, y, z };

            float screenX = 0.0f;
            float screenY = 0.0f;
            float depth = 0.0f;
            bool visible = false;

            if (g_viewPort) {
                RE::NiCamera::WorldPtToScreenPt3(
                    (float(*)[4])g_worldToCamMatrix,
                    *g_viewPort,
                    worldPos,
                    screenX,
                    screenY,
                    depth,
                    1e-5f);

                visible = depth >= 0.0f;
            }

            if (a_params.movie) {
                a_params.movie->CreateArray(a_params.retVal);
            }
            RE::GFxValue val;
            val.SetNumber(screenX);
            a_params.retVal->PushBack(val);
            val.SetNumber(1.0f - screenY);
            a_params.retVal->PushBack(val);
            val.SetNumber(depth);
            a_params.retVal->PushBack(val);
            val.SetBoolean(visible);
            a_params.retVal->PushBack(val);
        }
    };

    bool RegisterScaleform(RE::GFxMovieView* a_view, RE::GFxValue* a_root)
    {
        if (!a_view || !a_root) {
            return false;
        }

        RE::GFxValue global;
        a_root->GetMember("_global", &global);
        if (!global.IsObject()) {
            return false;
        }

        RE::GFxValue skse;
        if (!global.GetMember("skse", &skse) || !skse.IsObject()) {
            a_view->CreateObject(&skse);
            global.SetMember("skse", skse);
        }

        RE::GFxValue plugins;
        if (!skse.GetMember("plugins", &plugins) || !plugins.IsObject()) {
            a_view->CreateObject(&plugins);
            skse.SetMember("plugins", plugins);
        }

        RE::GFxValue plugin;
        if (!plugins.GetMember("FloatingDamageV2", &plugin) || !plugin.IsObject()) {
            a_view->CreateObject(&plugin);
            plugins.SetMember("FloatingDamageV2", plugin);
        }

        static auto handler = RE::make_gptr<WorldPtToScreenPt3Handler>();
        RE::GFxValue fnValue;
        a_view->CreateFunction(&fnValue, handler.get());
        plugin.SetMember("WorldPtToScreenPt3", fnValue);

        return true;
    }

    void Register()
    {
        if (auto scaleform = SKSE::GetScaleformInterface()) {
            scaleform->Register(RegisterScaleform, "FloatingDamageV2");
        }

        FloatingDamageMenu::Register();

        if (auto queue = RE::UIMessageQueue::GetSingleton()) {
            queue->AddMessage("FloatingDamageV2", RE::UI_MESSAGE_TYPE::kShow, nullptr);
        }

        logger::info("Registered scaleform menu");
    }
}

