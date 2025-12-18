// Copyright Sumit Das 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AgenticAI.generated.h"


USTRUCT(BlueprintType)
struct FChatEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString User;

	UPROPERTY(BlueprintReadWrite)
	FString Chat;
};

UCLASS()
class AAgenticAI : public AActor
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
