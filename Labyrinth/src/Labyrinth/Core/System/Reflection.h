#pragma once

#include <string_view>

#if defined __clang__ || defined __GNUC__
#    define LAB_PRETTY_FUNCTION __PRETTY_FUNCTION__
#    define LAB_PRETTY_FUNCTION_PREFIX '='
#    define LAB_PRETTY_FUNCTION_SUFFIX ']'
#elif defined _MSC_VER
#    define LAB_PRETTY_FUNCTION __FUNCSIG__
#    define LAB_PRETTY_FUNCTION_PREFIX '<'
#    define LAB_PRETTY_FUNCTION_SUFFIX '>'
#endif


namespace Labyrinth {

    namespace Reflection {

#if defined LAB_PRETTY_FUNCTION
        template<typename Type>
        [[nodiscard]] constexpr auto GetLongName() noexcept
        {
            std::string_view pretty_function{ LAB_PRETTY_FUNCTION };
            auto first = pretty_function.find_first_not_of(' ', pretty_function.find_first_of(LAB_PRETTY_FUNCTION_PREFIX) + 1);
            auto value = pretty_function.substr(first, pretty_function.find_last_of(LAB_PRETTY_FUNCTION_SUFFIX) - first);
            return value;
        }

        template<typename Type>
        [[nodiscard]] constexpr auto GetName() noexcept
        {
            std::string_view long_name = GetLongName<Type>();
            auto first = long_name.find_last_of("::");
            if (first == std::string_view::npos)
                first = long_name.find_last_of(' ');
            else
                first++;
            if (first == std::string_view::npos)
                return long_name;
            return long_name.substr(first, long_name.length() - first);
        }

        template<typename Type>
        [[nodiscard]] constexpr auto IsTypeClass() noexcept
        {
            std::string_view long_name = GetLongName<Type>();
            auto last = long_name.find_first_of(' ');
            if (last == std::string_view::npos)
                return false;
            auto value = long_name.substr(0, last);
            if (value == "class" || value == "struct")
                return true;
            return false;
        }
#endif

        template<typename T>
        struct IsPointer { static constexpr bool Value = false; };

        template<typename T>
        struct IsPointer<T*> { static constexpr bool Value = true; };

        template<typename T>
        constexpr bool IsConst() { return std::is_const<T>::value; };
    }

    template<typename T>
    struct TypeData
    {
#if defined LAB_PRETTY_FUNCTION
        inline static constexpr auto LongName = Reflection::GetLongName<T>();
        inline static constexpr auto Name = Reflection::GetName<T>();
        inline static constexpr bool IsObject = Reflection::IsTypeClass<T>();
#endif
        inline static constexpr bool IsPointer = Reflection::IsPointer<T>::Value;
        inline static constexpr bool IsConst = Reflection::IsConst<T>;
    };
}
