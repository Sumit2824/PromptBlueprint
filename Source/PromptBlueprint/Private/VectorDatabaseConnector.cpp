// Copyright Sumit Das 2025. All Rights Reserved.


#include "VectorDatabaseConnector.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Http.h"
#include "Json.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

bool UVectorDatabaseConnector::LoadDataToAstraDB(const FString& AstraDBApiEndpoint, const FString& AstraDBAccessToken, const FString& Keyspace, const FString& Collection, TArray<FString> InjestData)
{
    FHttpModule* Http = &FHttpModule::Get();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

    FStringFormatOrderedArguments Arguments;
    Arguments.Add(Keyspace);
    Arguments.Add(Collection);
	FString endpoinURL = AstraDBApiEndpoint;
	FString FormattedURL = FString::Printf(TEXT("%s/api/json/v1/%s/%s"), *endpoinURL, *Keyspace, *Collection);
    Request->SetURL(FormattedURL);
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Token"), AstraDBAccessToken);
    TSharedPtr<FJsonObject> RootObj = MakeShareable(new FJsonObject());
    TSharedPtr<FJsonObject> InsertManyObj = MakeShareable(new FJsonObject());
    TArray<TSharedPtr<FJsonValue>> DocumentArray;

    for (const FString& Chunk : InjestData)
    {
        TSharedPtr<FJsonObject> DocObj = MakeShareable(new FJsonObject());

        // Data for the LLM to read
        DocObj->SetStringField(TEXT("content"), Chunk);

        // Instructions for Astra to generate the 1024-dimension vector
        DocObj->SetStringField(TEXT("$vectorize"), Chunk);

        // Optional: Add a timestamp or source tag
        DocObj->SetStringField(TEXT("source"), TEXT("UnrealEngine_Ingest"));

        DocumentArray.Add(MakeShareable(new FJsonValueObject(DocObj)));
    }

    InsertManyObj->SetArrayField(TEXT("documents"), DocumentArray);
    RootObj->SetObjectField(TEXT("insertMany"), InsertManyObj);

    // 3. Serialize and Send
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootObj.ToSharedRef(), Writer);

    Request->SetContentAsString(OutputString);
    // Bind lambda for response handling
    Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful)
        {
        if (bWasSuccessful && Response.IsValid())
        {
            int32 ResponseCode = Response->GetResponseCode();
            FString ResponseContent = Response->GetContentAsString();

            if (ResponseCode == 200 || ResponseCode == 201)
            {
                UE_LOG(LogTemp, Display, TEXT("AstraDB insert successful: %s"), *ResponseContent);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("AstraDB insert failed with code %d: %s"), ResponseCode, *ResponseContent);
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("AstraDB HTTP request failed"));
        }
        });
    Request->ProcessRequest();
    return false;
}

void UVectorDatabaseConnector::QueryAstraDB(const FString& AstraDBApiEndpoint, const FString& AstraDBAccessToken, const FString& Database, const FString& Collection, const FString& QueryText, int32 TopK, FAstraDBQueryComplete OnComplete)
{
    FHttpModule* Http = &FHttpModule::Get();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

    // Build the URL: {endpoint}/api/json/v1/{keyspace}/{collection}
    FString endpointURL = AstraDBApiEndpoint;
    FString FormattedURL = FString::Printf(TEXT("%s/api/json/v1/%s/%s"), *endpointURL, *Database, *Collection);
    
    Request->SetURL(FormattedURL);
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Token"), AstraDBAccessToken);

    // Build the JSON payload for vector similarity search
    TSharedPtr<FJsonObject> RootObj = MakeShareable(new FJsonObject());
    TSharedPtr<FJsonObject> FindObj = MakeShareable(new FJsonObject());
    TSharedPtr<FJsonObject> SortObj = MakeShareable(new FJsonObject());
    TSharedPtr<FJsonObject> OptionsObj = MakeShareable(new FJsonObject());

    // Use $vectorize to let AstraDB generate the embedding from the query text
    SortObj->SetStringField(TEXT("$vectorize"), QueryText);
    
    FindObj->SetObjectField(TEXT("sort"), SortObj);
    
    // Set the limit to retrieve top K results
    OptionsObj->SetNumberField(TEXT("limit"), TopK);
    FindObj->SetObjectField(TEXT("options"), OptionsObj);
    
    RootObj->SetObjectField(TEXT("find"), FindObj);

    // Serialize the JSON
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootObj.ToSharedRef(), Writer);

    Request->SetContentAsString(OutputString);

    // Bind lambda for response handling with callback
    Request->OnProcessRequestComplete().BindLambda([OnComplete](FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            TArray<FString> OutResults;
            bool bSuccess = false;

            if (bWasSuccessful && Response.IsValid())
            {
                int32 ResponseCode = Response->GetResponseCode();
                FString ResponseContent = Response->GetContentAsString();

                if (ResponseCode == 200)
                {
                    UE_LOG(LogTemp, Display, TEXT("AstraDB query successful: %s"), *ResponseContent);

                    // Parse the JSON response
                    TSharedPtr<FJsonObject> JsonObject;
                    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

                    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
                    {
                        // Extract documents from the response
                        TSharedPtr<FJsonObject> DataObj = JsonObject->GetObjectField(TEXT("data"));
                        if (DataObj.IsValid())
                        {
                            const TArray<TSharedPtr<FJsonValue>>* DocumentsArray;
                            if (DataObj->TryGetArrayField(TEXT("documents"), DocumentsArray))
                            {
                                for (const TSharedPtr<FJsonValue>& DocValue : *DocumentsArray)
                                {
                                    TSharedPtr<FJsonObject> DocObj = DocValue->AsObject();
                                    if (DocObj.IsValid())
                                    {
                                        FString Content;
                                        if (DocObj->TryGetStringField(TEXT("content"), Content))
                                        {
                                            OutResults.Add(Content);
                                        }
                                    }
                                }
                                bSuccess = true;
                            }
                        }
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("AstraDB query failed with code %d: %s"), ResponseCode, *ResponseContent);
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("AstraDB query HTTP request failed"));
            }

            // Execute the callback with results
            if (OnComplete.IsBound())
            {
                OnComplete.Execute(bSuccess, OutResults);
            }
        });

    Request->ProcessRequest();
}
