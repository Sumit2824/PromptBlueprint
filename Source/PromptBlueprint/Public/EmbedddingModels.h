// Copyright Sumit Das 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmbedddingModels.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnEmbedingResponseDetailed, const FString&, ResponseContent, bool, bWasSuccessful, int32, ResponseCode);

UCLASS()
class PROMPTBLUEPRINT_API UEmbedddingModels : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
    UFUNCTION(BlueprintCallable, Category = "PromptBlueprint", meta = (AutoCreateRefTerm = "StructuredOutput"))
    static void LocalEmbeding(
        const FString& Url,
        const FString& InputMessage,
        const FString& ModelName,
        const FOnEmbedingResponseDetailed& Callback
    );
};
