#pragma once

namespace Hooks
{
    struct RestoreActorValueHook
    {
        static void thunk(RE::Actor* a_actor, RE::ACTOR_VALUE_MODIFIER a_modifier, RE::ActorValue a_value, float a_amount);
        static inline REL::Relocation<decltype(thunk)> func;
        static inline constexpr std::size_t idx = 0x06;
    };

    struct HandleHealthDamageHook
    {
        static void thunk(RE::Actor* a_actor, RE::Actor* a_attacker, float a_damage);
        static inline REL::Relocation<decltype(thunk)> func;
        static inline constexpr std::size_t idx = 0x104;
    };

    void Install();
}

