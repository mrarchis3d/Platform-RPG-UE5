// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterDebug.h"
#include "Data/BodyDataAsset.h"
#include "Data/EquipmentDataAsset.h"
#include "../PlatformGameLogs.h"


ACharacterDebug::ACharacterDebug()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACharacterDebug::BeginPlay()
{
	Super::BeginPlay();

	if (!PreviewBodyData)
	{
		UE_LOG(LogCharacterVisual, Warning,
			TEXT("[CharacterDebug] No PreviewBodyData assigned — cards won't be visible."));
		return;
	}

	InitializeBody(PreviewBodyData);

	for (const auto& Pair : PreviewEquipment)
	{
		if (Pair.Value)
		{
			Equip(Pair.Key, Pair.Value);
		}
	}

	UE_LOG(LogCharacterVisual, Log,
		TEXT("[CharacterDebug] Initialized with body '%s' and %d equipment items."),
		*PreviewBodyData->BodyID.ToString(),
		PreviewEquipment.Num());
}
