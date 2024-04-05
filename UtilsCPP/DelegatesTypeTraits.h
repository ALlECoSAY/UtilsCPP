#pragma once
#include <optional>


// Type trait to check if a type is std::optional
template<typename T>
struct is_optional : std::false_type {};

template<typename T>
struct is_optional<std::optional<T>> : std::true_type {};

template<typename T>
inline constexpr bool is_optional_v = is_optional<T>::value;

// Type trait to conditionally wrap a type in std::optional if it is not already and is not void
template<typename T, bool IsOptional = is_optional_v<T>, bool IsVoid = std::is_void_v<T>>
struct delegate_return_spec;

// Specialization for non-optional, non-void types
template<typename T>
struct delegate_return_spec<T, false, false> {
    using type = std::optional<T>;
};

// Specialization for optional types
template<typename T>
struct delegate_return_spec<T, true, false> {
    using type = T;
};

// Specialization for void types
template<typename T>
struct delegate_return_spec<T, false, true> {
    using type = void;
};

// Helper type template
template<typename T>
using delegate_return_spec_t = typename delegate_return_spec<T>::type;
