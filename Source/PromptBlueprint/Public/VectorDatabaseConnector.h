// Copyright Sumit Das 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VectorDatabaseConnector.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FAstraDBQueryComplete, bool, bSuccess, const TArray<FString>&, Results);

/**
 * 
 */
UCLASS()
class PROMPTBLUEPRINT_API UVectorDatabaseConnector : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "PromptBlueprint")
	static bool LoadDataToAstraDB(const FString& AstraDBApiEndpoint, const FString& AstraDBAccessToken, const FString& Database, const FString& Collection, TArray<FString> InjestData);
	
	UFUNCTION(BlueprintCallable, Category = "PromptBlueprint")
	static void QueryAstraDB(const FString& AstraDBApiEndpoint, const FString& AstraDBAccessToken, const FString& Database, const FString& Collection, const FString& QueryText, int32 TopK, FAstraDBQueryComplete OnComplete);
};
