#pragma once

template<typename Type>
struct FScopedLudeoDataReadWriteEnterObjectGuard
{
	template<typename... ParameterPackType>
	FScopedLudeoDataReadWriteEnterObjectGuard(const Type& InInstance, ParameterPackType&&... ParameterPack) :
		Instance(InInstance)
	{
		bHasEnteredObject = Instance.EnterObject(Forward<ParameterPackType>(ParameterPack)...);
		check(bHasEnteredObject);
	}

	~FScopedLudeoDataReadWriteEnterObjectGuard()
	{
		if(bHasEnteredObject)
		{
			check(Instance.LeaveObject());
		}
	}

	bool HasEnteredObject() const
	{
		return bHasEnteredObject;
	}

private:
	const Type& Instance;
	bool bHasEnteredObject;
};

template<typename Type, bool bCanEnterComponentCallFail>
struct FScopedLudeoDataReadWriteEnterComponentGuard
{
	template<typename... ParameterPackType>
	FScopedLudeoDataReadWriteEnterComponentGuard(const Type& InInstance, ParameterPackType&&... ParameterPack) :
		Instance(InInstance)
	{
		bHasEnteredComponent = Instance.EnterComponent(Forward<ParameterPackType>(ParameterPack)...);
		check(bCanEnterComponentCallFail || bHasEnteredComponent);
	}

	~FScopedLudeoDataReadWriteEnterComponentGuard()
	{
		if(bHasEnteredComponent)
		{
			check(Instance.LeaveComponent());
		}
	}

	bool HasEnteredComponent() const
	{
		return bHasEnteredComponent;
	}

private:
	const Type& Instance;
	bool bHasEnteredComponent;
};

template<typename Type>
struct FScopedWritableObjectBindPlayerGuard
{
	template<typename... ParameterPackType>
	FScopedWritableObjectBindPlayerGuard(const Type& InInstance, ParameterPackType&&... ParameterPack) :
		Instance(InInstance)
	{
		bHasBindedPlayer = Instance.BindPlayer(Forward<ParameterPackType>(ParameterPack)...);
		check(bHasBindedPlayer);
	}

	~FScopedWritableObjectBindPlayerGuard()
	{
		if (bHasBindedPlayer)
		{
			check(Instance.UnbindPlayer());
		}
	}

	bool HasBindedPlayer() const
	{
		return bHasBindedPlayer;
	}

private:
	const Type& Instance;
	bool bHasBindedPlayer;
};
