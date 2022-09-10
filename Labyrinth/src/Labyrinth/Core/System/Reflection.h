#pragma once

#include <string_view>

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#    define LAB_FUNC_SIGNATURE __PRETTY_FUNCTION__
#    define LAB_FUNC_SIGNATURE_PREFIX '='
#    define LAB_FUNC_SIGNATURE_SUFFIX ']'
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#    define LAB_FUNC_SIGNATURE __PRETTY_FUNCTION__
#    define LAB_FUNC_SIGNATURE_PREFIX '='
#    define LAB_FUNC_SIGNATURE_SUFFIX ']'
#elif (defined(__FUNCSIG__) || (_MSC_VER))
#    define LAB_FUNC_SIGNATURE __FUNCSIG__
#    define LAB_FUNC_SIGNATURE_PREFIX '<'
#    define LAB_FUNC_SIGNATURE_SUFFIX '>'
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#   define LAB_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#   define LAB_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#   define LAB_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#   define LAB_FUNC_SIG __func__
#else
#   define LAB_FUNC_SIG "LAB_FUNC_SIG unknown!"
#endif

namespace Labyrinth {

    namespace Reflection {

#if defined LAB_FUNC_SIGNATURE_PREFIX
        template<typename Type>
        [[nodiscard]] constexpr auto GetLongName() noexcept
        {
            std::string_view pretty_function{ LAB_FUNC_SIGNATURE };
            auto first = pretty_function.find_first_not_of(' ', pretty_function.find_first_of(LAB_FUNC_SIGNATURE_PREFIX) + 1);
            auto value = pretty_function.substr(first, pretty_function.find_last_of(LAB_FUNC_SIGNATURE_SUFFIX) - first);
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
#endif

        template<typename T, typename R>
        inline static constexpr bool IsSame() { return std::is_same<T, R>::value; }
    }

    template<typename T>
    struct TypeInfo
    {
#if defined LAB_FUNC_SIGNATURE_PREFIX
        inline static constexpr auto LongName = Reflection::GetLongName<T>();
        inline static constexpr auto Name = Reflection::GetName<T>();
#endif
        inline static constexpr bool IsObject = std::is_class<T>::value;
        inline static constexpr bool IsPointer = std::is_pointer<T>::value;
        inline static constexpr bool IsEnum = std::is_enum<T>::value;
        inline static constexpr bool IsArray = std::is_array<T>::value;
        inline static constexpr bool IsConst = std::is_const<T>::value;
        inline static constexpr bool IsStandard = std::is_standard_layout<T>::value;
        template<typename R>
        inline static constexpr bool IsSameAs = Reflection::IsSame<T, R>();
    };

    template<typename T>
    struct FunctionInfo;

    template<typename R, typename... Args>
    struct FunctionInfo<std::function<R(Args...)>>
    {
        static constexpr usize ArgC = sizeof...(Args);
        using ReturnType = R;

        template <usize i>
        struct Arg
        {
            using Type = typename std::tuple_element<i, std::tuple<Args...>>::type;
        };
    };
}
