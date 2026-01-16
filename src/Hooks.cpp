#include "Hooks.h"
#include "FloatingDamageManager.h"
#include "Settings.h"

namespace Hooks
{
    void RestoreActorValueHook::thunk(RE::Actor* a_actor, RE::ACTOR_VALUE_MODIFIER a_modifier, RE::ActorValue a_value, float a_amount)
    {
        func(a_actor, a_modifier, a_value, a_amount);

        if (!a_actor) {
            return;
        }

        if (a_value != RE::ActorValue::kHealth) {
            return;
        }

        if (a_amount == 0.0f) {
            return;
        }

        if (a_actor->IsDead()) {
            return;
        }

        auto* settings = Settings::GetSingleton();
        if (!settings->enabled) {
            return;
        }

        bool isHeal = a_amount > 0.0f;
        FloatingDamageManager::GetSingleton()->AddFloatingNumber(a_actor, a_amount, isHeal);
    }

    void Install()
    {
        REL::Relocation<std::uintptr_t> vtbl{ RE::VTABLE_Actor[0] };
        RestoreActorValueHook::func = vtbl.write_vfunc(RestoreActorValueHook::idx, RestoreActorValueHook::thunk);
        logger::info("Installed RestoreActorValue hook");
    }
}

