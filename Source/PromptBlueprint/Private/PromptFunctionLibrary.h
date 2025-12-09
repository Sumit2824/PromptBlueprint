// Copyright Sumit Das 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PromptFunctionLibrary.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EResponseType : uint8
{
    TEXT UMETA(DisplayName = "Text-based string file"),
    JSON UMETA(DisplayName = "Name of JSON file (e.g., \"example.json\") kept in the project's JSON folder"),
    TOON UMETA(DisplayName = "Toon (not implemented yet)")
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
       const FString& UserMessage,  
       const FString& SystemMessage,  
       const FString& ModelName,  
       const FString& ApiKey,  
       const EResponseType& ResponseType, 
       const TArray<FString>& AdditionalParams,
       const FOnChatResponseDetailed& Callback,
       const float Temparature = 0.3f
    );
	
	// Extract value from JSON string using a list of keys
	UFUNCTION(BlueprintCallable, Category = "PromptBlueprint")
	static bool ExtractValueFromChatJson(const FString& JsonString, const TArray<FString>& Keys, FString& OutValue);

	// Extract a chat output
	UFUNCTION(BlueprintCallable, Category = "PromptBlueprint")
	static FString ChatOutput(const FString& JsonString);

};
