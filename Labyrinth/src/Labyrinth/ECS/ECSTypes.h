#pragma once

#include <Labyrinth/Core/System/Types.h>

#include <typeindex>

namespace Labyrinth::ECS {

    using EntityID = i16;
    constexpr EntityID MAX_ENTITIES = Limits::i16Max;
    constexpr EntityID NullEnt = -1;

    using ComponentType = std::string;

    template<typename... Type>
    struct TypeList
    {
        static constexpr usize Size = sizeof...(Type);

        using TupleType = std::tuple<Type...>;
        
        template<typename T>
        static constexpr bool Contains = std::disjunction<std::is_same<T, Type>...>::value;

        template<usize I, typename T>
        static constexpr usize IndexFunction() 
        {
            LAB_STATIC_ASSERT(I < std::tuple_size<TupleType>::value, "The element is not in the tuple");

            using Val = typename std::tuple_element<I, TupleType>::type;
            if constexpr (TypeInfo<Val>::IsSameAs<T>) {
                return I;
            }
            else {
                return IndexFunction<I + 1, T, TupleType>();
            }
        }

        template<typename T>
        static constexpr usize Index = IndexFunction<0, T>();
    };
}