// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenRouterFunctionLibrary.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Http.h"
#include "Json.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"




void UOpenRouterFunctionLibrary::AIML_LLMs(const FString& Url, const FString& UserMessage, const FString& SystemMessage, const FString& ModelName, const FString& ApiKey, const TArray<FString>& AdditionalParams, const FOnChatResponseDetailed& Callback, const float Temparature)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	Request->SetURL(Url);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Connection"), TEXT("keep-alive"));
	Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *ApiKey));

	// Construct the JSON body
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetStringField("model", ModelName);

	float temp = FMath::Clamp(Temparature, 0.0f, 1.0f);
	JsonObject->SetNumberField("temperature", temp);

	//If JSONFilePath is provided, read the JSON file and set it in the request
	if (AdditionalParams.Num() > 0)
	{
		for (const FString& JSONFileName : AdditionalParams)
		{
			if (JSONFileName.IsEmpty())
			{
				UE_LOG(LogTemp, Warning, TEXT("Empty JSON file name provided."));
				continue;
			}
			// Load the JSON file content
			FString JsonContent;
			FString JSONFilePath = FPaths::ProjectContentDir() / TEXT("JSON") / JSONFileName; // JSON files are stored in a "JSON" folder in the Content directory
			if (FFileHelper::LoadFileToString(JsonContent, *JSONFilePath))
			{
				TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonContent);
				TSharedPtr<FJsonObject> FileJsonObject;
				if (FJsonSerializer::Deserialize(Reader, FileJsonObject) && FileJsonObject.IsValid())
				{
					// Merge the JSON file content into the main JsonObject
					for (const auto& Pair : FileJsonObject->Values)
					{
						JsonObject->SetField(Pair.Key, Pair.Value);
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to parse JSON file: %s"), *JSONFilePath);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to load JSON file: %s"), *JSONFilePath);
			}
		}
	}
	// Set up the request format
	/*TSharedPtr<FJsonObject> RequestFormat = MakeShared<FJsonObject>();
	RequestFormat->SetStringField("type", "json_object");
	JsonObject->SetObjectField("request_format", RequestFormat);*/



	/*TSharedPtr<FJsonObject> ResponseFormat = MakeShared<FJsonObject>();
	ResponseFormat->SetStringField("type", "json_object");
	JsonObject->SetObjectField("response_format", ResponseFormat);*/

	/*JsonObject->SetNumberField("top_p", 1.0);
	JsonObject->SetNumberField("max_tokens", 200);
	JsonObject->SetNumberField("presence_penalty", 0.5);
	JsonObject->SetNumberField("frequency_penalty", 0.0);*/

	TArray<TSharedPtr<FJsonValue>> Messages;

	// Add system message if provided
	if (!SystemMessage.IsEmpty())
	{
		TSharedPtr<FJsonObject> SystemMsg = MakeShared<FJsonObject>();
		SystemMsg->SetStringField("role", "system");
		SystemMsg->SetStringField("content", SystemMessage);
		Messages.Add(MakeShared<FJsonValueObject>(SystemMsg));
	}

	TSharedPtr<FJsonObject> UserMsg = MakeShared<FJsonObject>();
	UserMsg->SetStringField("role", "user");
	UserMsg->SetStringField("content", UserMessage);
	Messages.Add(MakeShared<FJsonValueObject>(UserMsg));
	JsonObject->SetArrayField("messages", Messages);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBody);
	UE_LOG(LogTemp, Display, TEXT("[PromptBlueprint] AIML_LLMs: Post %s body %s"),*Url, *RequestBody);
	// Bind response
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			FString ResponseStr = TEXT("No Response");
			int32 ResponseCode = -1;

			if (Response.IsValid())
			{
				ResponseStr = Response->GetContentAsString();
				ResponseCode = Response->GetResponseCode();
			}

			Callback.ExecuteIfBound(ResponseStr, bWasSuccessful, ResponseCode);
		});

	Request->ProcessRequest();
}

bool UOpenRouterFunctionLibrary::ExtractValueFromChatJson(const FString& JsonString, const TArray<FString>& Keys, FString& OutValue)
{
	OutValue = TEXT("");

	if (JsonString.IsEmpty() || Keys.Num() == 0)
	{
		OutValue = TEXT("Input String is Empty");
		return false;
	}

	TSharedPtr<FJsonObject> RootObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
	{
		OutValue = JsonString;
		return false;
	}

	TSharedPtr<FJsonValue> CurrentValue = MakeShareable(new FJsonValueObject(RootObject));

	for (const FString& Key : Keys)
	{
		// Handle null or invalid
		if (!CurrentValue.IsValid())
		{
			OutValue = FString::Printf(TEXT("Missing key: %s"), *Key);
			return false;
		}

		// If current is array: always take index 0
		if (CurrentValue->Type == EJson::Array)
		{
			const TArray<TSharedPtr<FJsonValue>>& Array = CurrentValue->AsArray();
			if (!Array.IsValidIndex(0))
			{
				OutValue = TEXT("Array at key is empty.");
				return false;
			}
			CurrentValue = Array[0];  // Always go to index 0
		}

		// If current is object: go to next key
		if (CurrentValue->Type == EJson::Object)
		{
			TSharedPtr<FJsonObject> Obj = CurrentValue->AsObject();
			if (!Obj.IsValid() || !Obj->HasField(Key))
			{
				OutValue = FString::Printf(TEXT("Key '%s' not found."), *Key);
				return false;
			}
			CurrentValue = Obj->TryGetField(Key);
		}
		else
		{
			OutValue = TEXT("Unexpected JSON structure.");
			return false;
		}
	}

	// Final extraction
	if (CurrentValue.IsValid())
	{
		switch (CurrentValue->Type)
		{
		case EJson::String:
			OutValue = CurrentValue->AsString();
			return true;
		case EJson::Number:
			OutValue = FString::SanitizeFloat(CurrentValue->AsNumber());
			return true;
		case EJson::Boolean:
			OutValue = CurrentValue->AsBool() ? TEXT("true") : TEXT("false");
			return true;
		default:
			// Serialize any other value types (objects, arrays)
			FString Output;
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
			if (FJsonSerializer::Serialize(CurrentValue, TEXT(""), Writer))
			{
				OutValue = Output;
				return true;
			}
		}
	}

	OutValue = TEXT("Unknown error.");
	return false;
}

FString UOpenRouterFunctionLibrary::ChatOutput(const FString& JsonString)
{
	if (JsonString.IsEmpty())
	{
		return FString();
	}
	TSharedPtr<FJsonObject> RootObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (FJsonSerializer::Deserialize(Reader, RootObject) && RootObject.IsValid())
	{
		if (RootObject->HasField(TEXT("choices")))
		{
			const TArray<TSharedPtr<FJsonValue>>& Choices = RootObject->GetArrayField(TEXT("choices"));
			if (Choices.Num() > 0 && Choices[0]->Type == EJson::Object)
			{
				TSharedPtr<FJsonObject> ChoiceObj = Choices[0]->AsObject();
				if (ChoiceObj->HasField(TEXT("message")) && ChoiceObj->GetObjectField(TEXT("message"))->HasField(TEXT("content")))
				{
					return ChoiceObj->GetObjectField(TEXT("message"))->GetStringField(TEXT("content"));
				}
			}
		}
	}
	return FString();
}

