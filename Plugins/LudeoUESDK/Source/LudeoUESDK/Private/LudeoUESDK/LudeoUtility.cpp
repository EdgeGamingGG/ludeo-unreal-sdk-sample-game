#pragma once

#include "LudeoUESDK/LudeoUtility.h"

const char* FLudeoUtility::FastConvertAttributeFNameToCString(const FName& Name)
{
	static TMap<FName, TArray<char>> Dictionary;

	if (const TArray<char>* pCString = Dictionary.Find(Name))
	{
		return pCString->GetData();
	}
	else
	{
		const FString NameString = Name.ToString();

		const FTCHARToUTF8 Converter
		(
			*NameString,
			NameString.Len()
		);

		TArray<char>& CString = Dictionary.Add(Name);
		CString.Append(Converter.Get(), Converter.Length());
		CString.Add('\0');

		return CString.GetData();
	}
}
