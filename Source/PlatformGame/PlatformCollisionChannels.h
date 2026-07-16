// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/EngineTypes.h"

/**
 * Custom collision channel aliases for the project.
 * TwoWayPlatform is registered in DefaultEngine.ini as ECC_GameTraceChannel1.
 */
namespace PlatformCollision
{
	/*The Two Way Platform Collision Channel*/
	constexpr ECollisionChannel TwoWayPlatform = ECC_GameTraceChannel1;
}
