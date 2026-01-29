// Copyright Sumit Das 2025. All Rights Reserved.


#include "TextSplitter.h"

TArray<FString> UTextSplitter::SplitText(const TArray<FString>& InputArray, const int32& ChunkOverlap, const int32 ChunkSize, const FString& Separator)
{

    TArray<FString> Result;

    if (InputArray.Num() == 0 || ChunkSize <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid input: empty array or invalid chunk size."));
        return Result;
    }

    // Load all files and combine their content
    FString CombinedText;
    for (const FString& FilePath : InputArray)
    {
        FString FileContent;
        if (FFileHelper::LoadFileToString(FileContent, *FilePath))
        {
            if (!CombinedText.IsEmpty() && !Separator.IsEmpty())
            {
                CombinedText += Separator;
            }
            CombinedText += FileContent;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to load file: %s"), *FilePath);
        }
    }

    if (CombinedText.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("No content loaded from files."));
        return Result;
    }

    // Split by separator if provided
    TArray<FString> SeparatedParts;
    if (!Separator.IsEmpty())
    {
        CombinedText.ParseIntoArray(SeparatedParts, *Separator, false);
    }
    else
    {
        SeparatedParts.Add(CombinedText);
    }

    // Split into chunks based on ChunkSize and ChunkOverlap
    FString CurrentChunk;
    int32 CurrentChunkLength = 0;

    for (int32 i = 0; i < SeparatedParts.Num(); ++i)
    {
        const FString& Part = SeparatedParts[i];

        // If adding this part exceeds chunk size, finalize current chunk
        if (CurrentChunkLength > 0 && CurrentChunkLength + Part.Len() + Separator.Len() > ChunkSize)
        {
            Result.Add(CurrentChunk);

            // Handle overlap: extract last ChunkOverlap characters for next chunk
            if (ChunkOverlap > 0 && CurrentChunk.Len() > ChunkOverlap)
            {
                CurrentChunk = CurrentChunk.Right(ChunkOverlap);
                CurrentChunkLength = CurrentChunk.Len();

                if (!Separator.IsEmpty())
                {
                    CurrentChunk += Separator;
                    CurrentChunkLength += Separator.Len();
                }
            }
            else
            {
                CurrentChunk.Empty();
                CurrentChunkLength = 0;
            }
        }

        // Add the current part
        if (CurrentChunkLength > 0 && !Separator.IsEmpty())
        {
            CurrentChunk += Separator;
            CurrentChunkLength += Separator.Len();
        }

        CurrentChunk += Part;
        CurrentChunkLength += Part.Len();
    }

    // Add any remaining chunk
    if (!CurrentChunk.IsEmpty())
    {
        Result.Add(CurrentChunk);
    }

    return Result;
}
