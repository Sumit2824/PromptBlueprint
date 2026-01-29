// Copyright Sumit Das 2025. All Rights Reserved.


#include "EmbedddingModels.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Http.h"
#include "Json.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

void UEmbedddingModels::LocalEmbeding(const FString& Url, const FString& InputMessage, const FString& ModelName, const FOnEmbedingResponseDetailed& Callback)
{
    FHttpModule* Http = &FHttpModule::Get();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

    // Set up the HTTP request
    Request->SetURL(Url);
    Request->SetVerb(TEXT("GET"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // Build the JSON payload for the embedding request
    TSharedPtr<FJsonObject> RootObj = MakeShareable(new FJsonObject());
    RootObj->SetStringField(TEXT("model"), ModelName);
    RootObj->SetStringField(TEXT("input"), InputMessage);

    // Serialize the JSON
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootObj.ToSharedRef(), Writer);

    Request->SetContentAsString(OutputString);

    // Bind lambda for response handling with callback
    Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            FString ResponseContent = TEXT("");
            int32 ResponseCode = 0;

            if (bWasSuccessful && Response.IsValid())
            {
                ResponseCode = Response->GetResponseCode();
                ResponseContent = Response->GetContentAsString();

                if (ResponseCode == 200)
                {
                    UE_LOG(LogTemp, Display, TEXT("Local embedding request successful: %s"), *ResponseContent);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Local embedding request failed with code %d: %s"), ResponseCode, *ResponseContent);
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Local embedding HTTP request failed"));
                ResponseContent = TEXT("HTTP request failed");
            }

            Callback.ExecuteIfBound(ResponseContent, bWasSuccessful, ResponseCode);

        });

    Request->ProcessRequest();
}
