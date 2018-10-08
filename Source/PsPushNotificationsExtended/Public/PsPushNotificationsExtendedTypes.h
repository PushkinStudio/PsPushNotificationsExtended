// Copyright 2018 Mail.Ru Group. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "PsPushNotificationsExtendedTypes.generated.h"

#define PS_PUSH_FUNC (FString(__FUNCTION__))	// Current Class Name + Function Name where this is called
#define PS_PUSH_LINE (FString::FromInt(__LINE__)) // Current Line Number in the code where this is called
#define PS_PUSH_FUNC_LINE (PS_PUSH_FUNC + "(" + PS_PUSH_LINE + ")") // Current Class and Line Number where this is called!

USTRUCT(BlueprintType)
struct FPsNotificationsAction
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Title;

	UPROPERTY(BlueprintReadWrite)
	FString ActionId;
};

USTRUCT(BlueprintType)
struct FPsNotification
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FText Title;

	UPROPERTY(BlueprintReadWrite)
	FText Subtitle;

	UPROPERTY(BlueprintReadWrite)
	FText Body;

	UPROPERTY(BlueprintReadWrite)
	FString Category;

	UPROPERTY(BlueprintReadWrite)
	FString ContentURL;

	UPROPERTY(BlueprintReadWrite)
	bool bLocalContent;

	UPROPERTY(BlueprintReadWrite)
	FString SoundName;

	UPROPERTY(BlueprintReadWrite)
	int32 BadgeNumber;

	UPROPERTY(BlueprintReadWrite)
	FString ActivationCode;

	FPsNotification() : bLocalContent(false), BadgeNumber(0) {}

	FPsNotification(const FText& Title_, const FText& Subtitle_, const FText& Body_,
		const FString& Category_, const FString& ContentURL_, bool bLocalContent_)
		: Title(Title_),
		  Subtitle(Subtitle_),
		  Body(Body_),
		  Category(Category_),
		  ContentURL(ContentURL_),
		  bLocalContent(bLocalContent_),
		  BadgeNumber(0)
	{}

	FPsNotification(const FText& Title_, const FText& Subtitle_, const FText& Body_,
		const FString& Category_, const FString& ContentURL_, bool bLocalContent_, int32 BadgeNumber_, const FString& SoundName_, const FString& ActivationCode_)
		: Title(Title_),
		  Subtitle(Subtitle_),
		  Body(Body_),
		  Category(Category_),
		  ContentURL(ContentURL_),
		  bLocalContent(bLocalContent_),
		  SoundName(SoundName_),
		  BadgeNumber(BadgeNumber_),
		  ActivationCode(ActivationCode_)
	{}
};
