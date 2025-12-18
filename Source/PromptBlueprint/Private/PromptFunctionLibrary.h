// Copyright Sumit Das 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AgenticAI.h"
#include "PromptFunctionLibrary.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EResponseType : uint8
{
    TEXT UMETA(DisplayName = "Text"),
    JSON UMETA(DisplayName = "JSON"),
    TOON UMETA(DisplayName = "Toon ")
};


DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnChatResponseDetailed, const FString&, ResponseContent, bool, bWasSuccessful, int32, ResponseCode);


UCLASS()
class UPromptFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// Generate Text using AIML LLMs
    UFUNCTION(BlueprintCallable, Category = "PromptBlueprint")  
    static void AIML_LLMs(  
       const FString& Url,  
       const FString& InputMessage, 
       const FString& ModelName,  
       const FString& ApiKey,  
       const FOnChatResponseDetailed& Callback,
       const TArray<FString>& AdditionalParams,
       const EResponseType& ResponseType = EResponseType::TEXT, 
       const float Temparature = 0.3f
    );
	
	// Extract value from JSON string using a list of keys
	UFUNCTION(BlueprintCallable, Category = "PromptBlueprint")
	static bool ExtractValueFromChatJson(const FString& JsonString, const TArray<FString>& Keys, FString& OutValue);

	// Extract a chat output
	UFUNCTION(BlueprintCallable, Category = "PromptBlueprint")
	static FString ChatOutput(const FString& JsonString);

	UFUNCTION(BlueprintCallable, Category = "PromptBlueprint")
	static FString MessageInput(const FString SystemMessage, const TArray<FChatEntry>& UserMessages);

};
