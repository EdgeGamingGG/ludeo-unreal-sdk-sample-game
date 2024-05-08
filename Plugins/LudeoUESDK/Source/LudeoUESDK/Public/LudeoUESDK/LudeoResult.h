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
	FORCEINLINE FLudeoResult(const LudeoResult Result = LudeoResult::Unknown) :
		ResultAsInteger(static_cast<int64>(Result))
	{

	}

	static FLudeoResult Success()
	{
		return FLudeoResult(LudeoResult::Success);
	}

	static FLudeoResult Failed(const LudeoResult InResult = LudeoResult::Unknown)
	{
		check(InResult != LudeoResult::Success);

		return FLudeoResult(InResult);
	}

	FORCEINLINE bool IsSuccessful() const
	{
		return (static_cast<LudeoResult>(ResultAsInteger) == LudeoResult::Success);
	}

	FORCEINLINE bool IsFailed() const
	{
		return !IsSuccessful();
	}

	FORCEINLINE operator LudeoResult() const
	{
		return static_cast<LudeoResult>(ResultAsInteger);
	}

	FORCEINLINE bool operator==(const LudeoResult OtherResult)
	{
		return (static_cast<LudeoResult>(ResultAsInteger) == OtherResult);
	}

	FORCEINLINE bool operator==(const FLudeoResult& OtherResult)
	{
		return (ResultAsInteger == OtherResult.ResultAsInteger);
	}

	TStringView<ANSICHAR> ToString() const
	{
		return ludeo_ResultToString(*this);
	}

private:
	UPROPERTY(Transient)
	int64 ResultAsInteger;
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
