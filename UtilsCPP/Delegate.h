#pragma once

#include <functional>
#include <memory>
#include <cassert>
#include <optional>

#pragma region Type traits

// Type trait to check if a type is std::optional
template<typename T>
struct is_optional : std::false_type {};

template<typename T>
struct is_optional<std::optional<T>> : std::true_type {};

template<typename T>
inline constexpr bool is_optional_v = is_optional<T>::value;

// Type trait to conditionally wrap a type in std::optional if it is not already and is not void
template<typename T, bool IsOptional = is_optional_v<T>, bool IsVoid = std::is_void_v<T>>
struct conditional_optional;

// Specialization for non-optional, non-void types
template<typename T>
struct conditional_optional<T, false, false> {
    using type = std::optional<T>;
};

// Specialization for optional types
template<typename T>
struct conditional_optional<T, true, false> {
    using type = T;
};

// Specialization for void types
template<typename T>
struct conditional_optional<T, false, true> {
    using type = void;
};

// Helper type template
template<typename T>
using conditional_optional_t = typename conditional_optional<T>::type;

#pragma endregion Type traits


template <typename Return, typename ...Args>
class Delegate
{
private:
    using Return_CondOptional = conditional_optional_t<Return>;
	using DelegateFunction = std::function<Return_CondOptional(Args...)>;

	DelegateFunction _DelegateFunction = nullptr;

public:

    void BindLambda(std::function<Return(Args...)> const& lambda)
    {
        _DelegateFunction = [lambda](Args&& ...args) -> Return_CondOptional {
                return lambda(std::forward<Args>(args)...);
            };
	}

    template<class Object>
    void BindObject(Object* const obj, Return(Object::* const method_ptr)(Args...))
    {
        _DelegateFunction = [obj, method_ptr](Args&& ...args) -> Return_CondOptional {
                return (obj->*method_ptr)(std::forward<Args>(args)...);
            };
    }

    template<class Object>
    void BindWeakObject(std::shared_ptr<Object> const spObj, Return(Object::* const method_ptr)(Args...))
    {
        std::weak_ptr<Object> wp = spObj;
        _DelegateFunction = [wp, method_ptr](Args&& ...args) -> Return_CondOptional {
                if (auto sp = wp.lock())
                {
					return  (sp.get()->*method_ptr)(std::forward<Args>(args)...) ;
				}
                if constexpr (!std::is_void_v<Return>)
				{
                    return std::nullopt;
                }
			};
    }

    template<class Object>
    void BindStrongObject(std::shared_ptr<Object> const spObj, Return(Object::* const method_ptr)(Args...))
    {
        _DelegateFunction = [spObj, method_ptr](Args&& ...args) -> Return_CondOptional {
                return (spObj.get()->*method_ptr)(std::forward<Args>(args)...);
            };
    }

    bool IsBound() const
    {
		return _DelegateFunction != nullptr;
	}

    void Unbind()
    {
        _DelegateFunction = nullptr;
    }

    Return_CondOptional Execute(Args&& ...args) const
    {
        assert(_DelegateFunction);
        return _DelegateFunction(std::forward<Args>(args)...);
    }

    Return_CondOptional ExecuteIfBound(Args&& ...args) const {
        if (_DelegateFunction)
        {
			return _DelegateFunction(std::forward<Args>(args)...);
        }
        if constexpr (!std::is_void_v<Return>)
        {
			return std::nullopt;
		}
    }
};
