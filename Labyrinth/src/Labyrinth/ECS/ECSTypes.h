#pragma once

#include <Labyrinth/Core/System/Types.h>

#include <typeindex>

namespace Labyrinth::ECS {

    using EntityID = i16;
    constexpr EntityID MAX_ENTITIES = Limits::i16Max;
    constexpr EntityID NullEnt = -1;

    using ComponentType = std::string;

    namespace TypeUtils {

        template<usize I, typename T, typename TupleType>
        inline static constexpr usize IndexFunction()
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
    }

    template<typename... Types>
    struct TypeList
    {
        static constexpr usize Size = sizeof...(Types);

        using TupleType = std::tuple<Types...>;
        
        template<typename T>
        static constexpr bool Contains = std::disjunction<std::is_same<T, Types>...>::value;

        template<typename T>
        static constexpr usize Index = TypeUtils::IndexFunction<0, T, TupleType>();

        template<usize I>
        using Type = typename std::tuple_element<I, TupleType>::type;

    };
}