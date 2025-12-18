// Copyright Sumit Das 2025. All Rights Reserved.


#include "AgenticAI.h"

// Sets default values
AAgenticAI::AAgenticAI()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AAgenticAI::BeginPlay()
{
	Super::BeginPlay();
	ChatHistory.Empty();
}

// Called every frame
void AAgenticAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAgenticAI::SaveToMemory(FString user, FString chat)
{
	FChatEntry newEntry;
	newEntry.User = user;
	newEntry.Chat = chat;
	ChatHistory.Add(newEntry);
}

void AAgenticAI::SaveAiResponseToMemory(FString response)
{
	SaveToMemory("assistant", response);
}

TArray<FChatEntry> AAgenticAI::LoadFromMemory(int32 NoOfChats)
{
	TArray<FChatEntry> recentChats;
	for (int32 i = FMath::Max(0, ChatHistory.Num() - NoOfChats); i < ChatHistory.Num(); i++)
	{
		recentChats.Add(ChatHistory[i]);
	}
	return recentChats;
}

