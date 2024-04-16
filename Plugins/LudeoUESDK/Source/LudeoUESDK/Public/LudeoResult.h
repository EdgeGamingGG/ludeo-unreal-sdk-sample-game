#pragma once

#include "CoreMinimal.h"

#include <Ludeo/Common.h>
#include <Ludeo/Init.h>

#include "LudeoResult.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Result"))
struct LUDEOUESDK_API FLudeoResult
{
	GENERATED_BODY()

public:
	FORCEINLINE FLudeoResult(const LudeoResult InResult = LudeoResult::Unknown) :
		Result(InResult)
	{

	}

	FORCEINLINE bool IsSuccessful() const
	{
		return (Result == LudeoResult::Success);
	}

	FORCEINLINE bool IsFailed() const
	{
		return !IsSuccessful();
	}

	FORCEINLINE operator LudeoResult() const
	{
		return Result;
	}

	FORCEINLINE bool operator==(const LudeoResult OtherResult)
	{
		return (Result == OtherResult);
	}

	FORCEINLINE bool operator==(const FLudeoResult& OtherResult)
	{
		return (Result == OtherResult.Result);
	}

	TStringView<ANSICHAR> ToString() const
	{
		return ludeo_ResultToString(Result);
	}

private:
	LudeoResult Result;
};

template<typename DataType>
struct TOptionalWithLudeoResult : public FLudeoResult, private TOptional<DataType>
{
public:
	template<typename... ParameterPackType>
	TOptionalWithLudeoResult(const FLudeoResult& InResult, ParameterPackType&&... ParameterPack) :
		FLudeoResult(InResult)
	{
		if (IsSuccessful())
		{
			TOptional<DataType>::Emplace(Forward<ParameterPackType>(ParameterPack)...);
		}
	}

	~TOptionalWithLudeoResult() = default;

	DataType& GetValue()
	{
		return TOptional<DataType>::GetValue();
	}

	const DataType& GetValue() const
	{
		return TOptional<DataType>::GetValue();
	}
};
