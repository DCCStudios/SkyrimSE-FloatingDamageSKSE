#pragma once

namespace Hooks
{
    struct RestoreActorValueHook
    {
        static void thunk(RE::Actor* a_actor, RE::ACTOR_VALUE_MODIFIER a_modifier, RE::ActorValue a_value, float a_amount);
        using func_t = decltype(thunk);
        static inline func_t* funcActor = nullptr;
        static inline func_t* funcCharacter = nullptr;
        static inline func_t* funcPlayer = nullptr;
        static inline constexpr std::size_t idx = 0x06;
    };

    struct HandleHealthDamageHook
    {
        static void thunk(RE::Actor* a_actor, RE::Actor* a_attacker, float a_damage);
        using func_t = decltype(thunk);
        static inline func_t* funcActor = nullptr;
        static inline func_t* funcCharacter = nullptr;
        static inline func_t* funcPlayer = nullptr;
        static inline constexpr std::size_t idx = 0x104;
    };

    void Install();
}

