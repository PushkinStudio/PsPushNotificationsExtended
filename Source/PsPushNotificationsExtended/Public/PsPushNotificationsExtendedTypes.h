// Copyright 2018 Mail.Ru Group. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "PsPushNotificationsExtendedTypes.generated.h"

USTRUCT(BlueprintType)
struct FPsNotificationsAction
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ActionId;
};

USTRUCT(BlueprintType)
struct FPsNotification
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Subtitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Body;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ContentURL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLocalContent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SoundName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BadgeNumber;

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
		const FString& Category_, const FString& ContentURL_, bool bLocalContent_, int32 BadgeNumber_, const FString& SoundName_)
		: Title(Title_),
		  Subtitle(Subtitle_),
		  Body(Body_),
		  Category(Category_),
		  ContentURL(ContentURL_),
		  bLocalContent(bLocalContent_),
		  SoundName(SoundName_),
		  BadgeNumber(BadgeNumber_)
	{}
};
