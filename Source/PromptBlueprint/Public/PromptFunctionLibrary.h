// Copyright Sumit Das 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AgenticAI.h"
#include "PromptFunctionLibrary.generated.h"



DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnChatResponseDetailed, const FString&, ResponseContent, bool, bWasSuccessful, int32, ResponseCode);


UCLASS()
class PROMPTBLUEPRINT_API UPromptFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// Generate Text using AIML LLMs
    UFUNCTION(BlueprintCallable, Category = "PromptBlueprint", meta = (AutoCreateRefTerm = "StructuredOutput"))
    static void AIML_LLMs(  
       const FString& Url,  
       const FString& InputMessage, 
       const FString& ModelName,  
       const FString& ApiKey,  
       const FOnChatResponseDetailed& Callback,
	   const FStructuredOutputOptions& StructuredOutput,
       const float Temparature = 0.3f
    );

    UFUNCTION(BlueprintCallable, Category = "PromptBlueprint", meta = (AutoCreateRefTerm = "FunctionCalls"))
    static void LLMFunctionCalling(
        const FString& Url,
        const FString& InputMessage,
        const FString& ModelName,
        const FString& ApiKey,
        const FOnChatResponseDetailed& Callback,
        const FFunctionCalling& FunctionCalls,
        const float Temparature = 0.3f
    );
    UFUNCTION(BlueprintCallable, Category = "PromptBlueprint", meta = (AutoCreateRefTerm = "StructuredOutput"))
    static void CodeInterpreter(
        const FString& Url,
        const FString& InputMessage,
        const FString& ModelName,
        const FString& ApiKey,
        const FOnChatResponseDetailed& Callback,
        const float Temparature = 0.3f
    );
	
	// Extract value from JSON string using a list of keys
	UFUNCTION(BlueprintCallable, Category = "PromptBlueprint")
	static bool ExtractValueFromChatJson(const FString& JsonString, const TArray<FString>& Keys, FString& OutValue);

	// Extract a chat output
	UFUNCTION(BlueprintCallable, Category = "PromptBlueprint")
	static FString ChatOutput(const FString& JsonString);

    // Extract a Function output
    UFUNCTION(BlueprintCallable, Category = "PromptBlueprint")
    static FFunctionOutput FunctionOutput(const FString& JsonString);

	UFUNCTION(BlueprintCallable, Category = "PromptBlueprint")
	static FString MessageInput(const FString SystemMessage, const TArray<FChatEntry>& UserMessages);

};
