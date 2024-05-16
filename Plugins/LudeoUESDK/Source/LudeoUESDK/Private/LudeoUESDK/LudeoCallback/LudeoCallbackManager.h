#pragma once

#include "Templates/UniquePtr.h"

template<typename T>
struct FLudeoFunctionTrait
    : FLudeoFunctionTrait<decltype(&T::operator())>
{};

template<typename ReturnType, typename... Args>
struct FLudeoFunctionTrait<ReturnType(*)(Args...)>
{
    using Signature = ReturnType(Args...);
};

template<typename ClassType, typename ReturnType, typename... Args>
struct FLudeoFunctionTrait<ReturnType(ClassType::*)(Args...) const>
{
    using Signature = ReturnType(Args...);
};

template<typename ClassType, typename ReturnType, typename... Args>
struct FLudeoFunctionTrait<ReturnType(ClassType::*)(Args...)>
{
    using Signature = ReturnType(Args...);
};

class FLudeoCallbackInterface
{
public:
    virtual ~FLudeoCallbackInterface() = default;
};

template<typename FunctionType>
class FLudeoCallback : public FLudeoCallbackInterface
{
public:
	FLudeoCallback(FunctionType&& InFunction) :
		Function(MoveTemp(InFunction))
	{

	}

	virtual ~FLudeoCallback() override = default;

	template<typename... ArgumentsType>
	void Invoke(ArgumentsType&&... Arguments)
	{
		Function(Forward<ArgumentsType>(Arguments)...);
	}

private:
	FunctionType Function;
};

class FLudeoCallbackManager
{
public:
	static FLudeoCallbackManager& GetInstance()
	{
		static FLudeoCallbackManager CallbackManager;
		return CallbackManager;
	}

	template<typename FunctionType>
	void* CreateCallback(FunctionType&& Function)
	{
		using FunctionSignature = typename FLudeoFunctionTrait<typename TDecay<FunctionType>::Type>::Signature;

		TUniquePtr<FLudeoCallbackInterface>& Callback = CallbackCollection.Emplace_GetRef
		(
			MakeUnique<FLudeoCallback<TFunction<FunctionSignature>>>(MoveTemp(Function))
		);

		return Callback.Get();
	}

	template<typename CallbackParameterType>
	bool InvokeAndRemoveCallback(const CallbackParameterType* CallbackParameter)
	{
		if(CallbackParameter != nullptr)
		{
			const int32 Index = CallbackCollection.IndexOfByPredicate([&](const TUniquePtr<FLudeoCallbackInterface>& Callback)
			{
				return (Callback.Get() == CallbackParameter->clientData);
			});

			if (CallbackCollection.IsValidIndex(Index))
			{
				auto Callback = static_cast<FLudeoCallback<TFunction<void(const CallbackParameterType&)>>*>(CallbackCollection[Index].Get());
				Callback->Invoke(*CallbackParameter);

				CallbackCollection.RemoveAtSwap(Index);

				return true;
			}
		}

		return false;
	}

	void Finalize()
	{
		CallbackCollection.Empty();
	}

private:
	TArray<TUniquePtr<FLudeoCallbackInterface>> CallbackCollection;
};
