#pragma once

#include <functional>
#include <memory>
#include <cassert>
#include <optional>

#include "DelegatesTypeTraits.h"


template <typename TReturnType, typename ...TArgsTypes>
class Delegate
{
private:
    using FReturnType = delegate_return_spec_t<TReturnType>;
	using FDelegateLambdaType = std::function<FReturnType(TArgsTypes...)>;
    template<typename TObjectType>
    using FMemberFunctionPtrType = TReturnType(TObjectType::*)(TArgsTypes...);



	FDelegateLambdaType _delegateLambda = nullptr;
    
public:

    void BindLambda(std::function<TReturnType(TArgsTypes...)> const& lambda)
    {
        _delegateLambda = [lambda](TArgsTypes&& ...args) -> FReturnType {
                return lambda(std::forward<TArgsTypes>(args)...);
            };
	}

    template<class TObjectType>
    void BindObject(TObjectType* const obj, FMemberFunctionPtrType<TObjectType> method_ptr)
    {
        _delegateLambda = [obj, method_ptr](TArgsTypes&& ...args) -> FReturnType {
                return (obj->*method_ptr)(std::forward<TArgsTypes>(args)...);
            };
    }

    template<class TObjectType>
    void BindWeakObject(std::shared_ptr<TObjectType> const spObj, FMemberFunctionPtrType<TObjectType> method_ptr)
    {
        std::weak_ptr<TObjectType> wp = spObj;
        _delegateLambda = [wp, method_ptr](TArgsTypes&& ...args) -> FReturnType {
                if (auto sp = wp.lock())
                {
					return  (sp.get()->*method_ptr)(std::forward<TArgsTypes>(args)...) ;
				}
                if constexpr (!std::is_void_v<TReturnType>)
				{
                    return std::nullopt;
                }
			};
    }

    template<class TObjectType>
    void BindStrongObject(std::shared_ptr<TObjectType> const spObj, FMemberFunctionPtrType<TObjectType> method_ptr)
    {
        _delegateLambda = [spObj, method_ptr](TArgsTypes&& ...args) -> FReturnType {
                return (spObj.get()->*method_ptr)(std::forward<TArgsTypes>(args)...);
            };
    }

    bool IsBound() const
    {
		return _delegateLambda != nullptr;
	}

    void Unbind()
    {
        _delegateLambda = nullptr;
    }

    FReturnType Execute(TArgsTypes&& ...args) const
    {
        assert(_delegateLambda);
        return _delegateLambda(std::forward<TArgsTypes>(args)...);
    }

    FReturnType ExecuteIfBound(TArgsTypes&& ...args) const {
        if (_delegateLambda)
        {
			return _delegateLambda(std::forward<TArgsTypes>(args)...);
        }
        if constexpr (!std::is_void_v<TReturnType>)
        {
			return std::nullopt;
		}
    }
};

//TODO: thread safe shared pointer binding