// Copyright Sumit Das 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TextSplitter.generated.h"

/**
 * 
 */
UCLASS()
class PROMPTBLUEPRINT_API UTextSplitter : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "PromptBlueprint")
	static TArray<FString> SplitText(const TArray<FString>& InputArray, const int32& ChunkOverlap, const int32 ChunkSize, const FString& Separator);
	
};
