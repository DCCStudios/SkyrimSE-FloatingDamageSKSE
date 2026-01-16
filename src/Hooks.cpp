#include "Hooks.h"
#include "FloatingDamageManager.h"
#include "Settings.h"

namespace Hooks
{
    void RestoreActorValueHook::thunk(RE::Actor* a_actor, RE::ACTOR_VALUE_MODIFIER a_modifier, RE::ActorValue a_value, float a_amount)
    {
        if (a_actor && a_actor->IsPlayerRef() && funcPlayer) {
            funcPlayer(a_actor, a_modifier, a_value, a_amount);
        } else if (a_actor && a_actor->As<RE::Character>() && funcCharacter) {
            funcCharacter(a_actor, a_modifier, a_value, a_amount);
        } else if (funcActor) {
            funcActor(a_actor, a_modifier, a_value, a_amount);
        }

        if (!a_actor) {
            return;
        }

        if (a_value != RE::ActorValue::kHealth) {
            return;
        }

        if (a_amount <= 0.0f) {
            return;
        }

        if (a_actor->IsDead()) {
            return;
        }

        auto* settings = Settings::GetSingleton();
        if (!settings->enabled) {
            return;
        }

        FloatingDamageManager::GetSingleton()->AddFloatingNumber(a_actor, a_amount, true);
    }

    void HandleHealthDamageHook::thunk(RE::Actor* a_actor, RE::Actor* a_attacker, float a_damage)
    {
        if (a_actor && a_actor->IsPlayerRef() && funcPlayer) {
            funcPlayer(a_actor, a_attacker, a_damage);
        } else if (a_actor && a_actor->As<RE::Character>() && funcCharacter) {
            funcCharacter(a_actor, a_attacker, a_damage);
        } else if (funcActor) {
            funcActor(a_actor, a_attacker, a_damage);
        }

        if (!a_actor) {
            return;
        }

        if (a_damage == 0.0f) {
            return;
        }

        if (a_actor->IsDead()) {
            return;
        }

        if (a_actor->IsPlayerRef()) {
            return;
        }

        auto* settings = Settings::GetSingleton();
        if (!settings->enabled || !settings->showDamage) {
            return;
        }

        FloatingDamageManager::GetSingleton()->AddFloatingNumber(a_actor, std::abs(a_damage), false);
    }

    void Install()
    {
        REL::Relocation<std::uintptr_t> actorVtbl{ RE::VTABLE_Actor[0] };
        RestoreActorValueHook::funcActor = reinterpret_cast<RestoreActorValueHook::func_t*>(
            actorVtbl.write_vfunc(RestoreActorValueHook::idx, RestoreActorValueHook::thunk));
        HandleHealthDamageHook::funcActor = reinterpret_cast<HandleHealthDamageHook::func_t*>(
            actorVtbl.write_vfunc(HandleHealthDamageHook::idx, HandleHealthDamageHook::thunk));

        REL::Relocation<std::uintptr_t> characterVtbl{ RE::VTABLE_Character[0] };
        RestoreActorValueHook::funcCharacter = reinterpret_cast<RestoreActorValueHook::func_t*>(
            characterVtbl.write_vfunc(RestoreActorValueHook::idx, RestoreActorValueHook::thunk));
        HandleHealthDamageHook::funcCharacter = reinterpret_cast<HandleHealthDamageHook::func_t*>(
            characterVtbl.write_vfunc(HandleHealthDamageHook::idx, HandleHealthDamageHook::thunk));

        REL::Relocation<std::uintptr_t> playerVtbl{ RE::VTABLE_PlayerCharacter[0] };
        RestoreActorValueHook::funcPlayer = reinterpret_cast<RestoreActorValueHook::func_t*>(
            playerVtbl.write_vfunc(RestoreActorValueHook::idx, RestoreActorValueHook::thunk));
        HandleHealthDamageHook::funcPlayer = reinterpret_cast<HandleHealthDamageHook::func_t*>(
            playerVtbl.write_vfunc(HandleHealthDamageHook::idx, HandleHealthDamageHook::thunk));

        logger::info("Installed RestoreActorValue and HandleHealthDamage hooks (Actor/Character/Player)");
    }
}

