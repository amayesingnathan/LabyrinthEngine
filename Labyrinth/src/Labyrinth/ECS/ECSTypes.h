#pragma once

#include <Labyrinth/Core/System/Types.h>

#include <typeindex>

namespace Labyrinth::ECS {

    using EntityID = i16;
    constexpr EntityID MAX_ENTITIES = Limits::i16Max;
    constexpr EntityID NullEnt = -1;

    using ComponentType = std::type_index;

    template<typename... Type>
    struct TypeList
    {
        inline static constexpr usize Size = sizeof...(Type);
        using Tuple = std::tuple<Type...>;
    };
}