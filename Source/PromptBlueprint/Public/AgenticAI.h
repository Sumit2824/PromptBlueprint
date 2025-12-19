// Copyright Sumit Das 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AgenticAI.generated.h"

UENUM(BlueprintType)
enum class EResponseType : uint8
{
	TEXT UMETA(DisplayName = "Text"),
	JSON UMETA(DisplayName = "JSON"),
	TOON UMETA(DisplayName = "Toon ")
};

USTRUCT(BlueprintType)
struct PROMPTBLUEPRINT_API FChatEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString User;

	UPROPERTY(BlueprintReadWrite)
	FString Chat;
};

USTRUCT(BlueprintType)
struct PROMPTBLUEPRINT_API FStructuredOutputOptions
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	bool IsStructuredOutput = false;

	UPROPERTY(BlueprintReadWrite)
	bool LoadFile = false;

	UPROPERTY(BlueprintReadWrite)
	EResponseType ResponseType = EResponseType::JSON;

	UPROPERTY(BlueprintReadWrite)
	FString SchemaFile = "";
};


UCLASS()
class PROMPTBLUEPRINT_API AAgenticAI : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAgenticAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "PromptBlueprint")
	virtual void SaveToMemory(FString user, FString chat);

	UFUNCTION(BlueprintCallable, Category = "PromptBlueprint")
	virtual void SaveAiResponseToMemory(FString response);

	UFUNCTION(BlueprintCallable, Category = "PromptBlueprint")
	virtual TArray<FChatEntry> LoadFromMemory(int32 NoOfChats);

private:
	TArray<FChatEntry> ChatHistory;
};
