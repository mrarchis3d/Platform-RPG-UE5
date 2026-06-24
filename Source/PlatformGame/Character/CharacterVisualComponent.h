// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ECharacterSlots.h"
#include "CharacterSlotStructs.h"
#include "CharacterVisualComponent.generated.h"

class UBodyDataAsset;
class UEquipmentDataAsset;
class USkeletalMeshComponent;
class UStaticMesh;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class UStaticMeshComponent;

UCLASS( ClassGroup=(Character), meta=(BlueprintSpawnableComponent) )
class PLATFORMGAME_API UCharacterVisualComponent : public USceneComponent
{
    GENERATED_BODY()

public:

    /** Sets the skeletal mesh whose bones will be used for plane attachment. */
    UFUNCTION(BlueprintCallable, Category = "Character|Visual",
              meta = (DisplayName = "Set Target Skeleton"))
    void SetTargetSkeleton(USkeletalMeshComponent* InSkeleton);

    /** Creates body-part planes from a BodyDataAsset and attaches them to the skeleton bones. */
    UFUNCTION(BlueprintCallable, Category = "Character|Visual",
              meta = (DisplayName = "Initialize Body"))
    void InitializeBody(UBodyDataAsset* BodyDataAsset);

    /** Creates an equipment plane for the given slot and hides covered body slots. */
    UFUNCTION(BlueprintCallable, Category = "Character|Visual",
              meta = (DisplayName = "Equip Visual"))
    void EquipVisual(EEquipmentSlotType Slot, UEquipmentDataAsset* EquipmentData);

    /** Removes an equipment plane and reveals any body slots it was hiding. */
    UFUNCTION(BlueprintCallable, Category = "Character|Visual",
              meta = (DisplayName = "Unequip Visual"))
    void UnequipVisual(EEquipmentSlotType Slot);

    /** Removes all equipment visuals and reveals all body slots. */
    UFUNCTION(BlueprintCallable, Category = "Character|Visual",
              meta = (DisplayName = "Clear All Equipment Visuals"))
    void ClearAllEquipmentVisuals();

    /** Sets a specific body-part plane visible or hidden. */
    UFUNCTION(BlueprintCallable, Category = "Character|Visual",
              meta = (DisplayName = "Set Body Plane Visible"))
    void SetBodyPlaneVisible(EBodySlotType Slot, bool bShow);

    /** Re-evaluates visibility of a body slot based on current equipment. */
    UFUNCTION(BlueprintCallable, Category = "Character|Visual",
              meta = (DisplayName = "Refresh Body Slot Visibility"))
    void RefreshBodySlotVisibility(EBodySlotType Slot);

    // ── Debug Tools (use during Play to adjust without restarting) ──────────

    /** Moves/rotates a body-slot plane in real time. Call from Blueprint during play.
     *  Once you find the right values, copy them into the DataAsset. */
    UFUNCTION(BlueprintCallable, Category = "Character|Visual|Debug",
              meta = (DisplayName = "[Debug] Adjust Body Slot",
                      DevelopmentOnly))
    void DebugAdjustBodySlot(EBodySlotType Slot, FVector NewLocation, FRotator NewRotation);

    /** Moves/rotates an equipment-slot plane in real time. */
    UFUNCTION(BlueprintCallable, Category = "Character|Visual|Debug",
              meta = (DisplayName = "[Debug] Adjust Equipment Slot",
                      DevelopmentOnly))
    void DebugAdjustEquipmentSlot(EEquipmentSlotType Slot, FVector NewLocation, FRotator NewRotation);

    /** Prints the world & relative transform of every active plane to the Output Log. */
    UFUNCTION(BlueprintCallable, Category = "Character|Visual|Debug",
              meta = (DisplayName = "[Debug] Print All Slot Transforms",
                      DevelopmentOnly))
    void DebugPrintAllSlotTransforms();

    /** Change PixelsPerUnit at runtime and re-scale all existing planes. */
    UFUNCTION(BlueprintCallable, Category = "Character|Visual|Debug",
              meta = (DisplayName = "[Debug] Set Pixels Per Unit",
                      DevelopmentOnly))
    void DebugSetPixelsPerUnit(float NewPPU);

    /** Material used when no specific template is provided. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Visual",
              meta = (DisplayName = "Fallback Material"))
    TObjectPtr<UMaterialInterface> FallbackMaterial = nullptr;

    /** How many pixels of the source sprite equal 1 Unreal Unit (1 cm).
     *  Example: PPU=5 and a 500x600px sprite → 100x120 UU (1m x 1.2m).
     *  Increase PPU to make sprites smaller, decrease to make them larger. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Visual",
              meta = (DisplayName = "Pixels Per Unit", ClampMin = "0.01"))
    float PixelsPerUnit = 5.f;

protected:

    UPROPERTY(Transient)
    TMap<EBodySlotType, TObjectPtr<UStaticMeshComponent>> BodyPlanes;

    UPROPERTY(Transient)
    TMap<EEquipmentSlotType, TObjectPtr<UStaticMeshComponent>> EquipmentPlanes;

    UPROPERTY(Transient)
    TMap<EBodySlotType, TObjectPtr<UMaterialInstanceDynamic>> BodyMaterials;

    UPROPERTY(Transient)
    TMap<EEquipmentSlotType, TObjectPtr<UMaterialInstanceDynamic>> EquipmentMaterials;

    UPROPERTY(Transient)
    TMap<EBodySlotType, EEquipmentSlotType> HiddenByEquipment;

    UPROPERTY(Transient)
    TObjectPtr<UStaticMesh> PlaneMesh = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBodyDataAsset> ActiveBodyData = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UMaterialInterface> BodyMaterialTemplate = nullptr;

    /** Skeleton whose bones are used for plane attachment. Set via SetTargetSkeleton(). */
    UPROPERTY(Transient)
    TObjectPtr<USkeletalMeshComponent> TargetSkeleton = nullptr;


    void DestroyAllPlanes();
    UStaticMeshComponent* CreateSlotPlane(const FName& SocketName, const FCharacterSlotConfig& Config, int32 ZOrder);
    UMaterialInstanceDynamic* CreateBodySlotMaterial(UMaterialInterface* Template, UStaticMeshComponent* Plane);
    UMaterialInstanceDynamic* CreateEquipmentSlotMaterial(UMaterialInterface* Template, UStaticMeshComponent* Plane);
    void HideBodySlot(EBodySlotType Slot, EEquipmentSlotType ByEquipment);
    void RevealBodySlot(EBodySlotType Slot);

    void EnsurePlaneMeshLoaded();
};
