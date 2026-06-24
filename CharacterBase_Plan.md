# CharacterBase — 2D Planes + 3D Rig System

## Overview

Sistema de personaje basado en **2D planes superpuestos** controlados por las transformaciones de huesos (bones/sockets) de un SkeletalMesh 3D. Cada layer (body part / equipment) es un `UStaticMeshComponent` con un material que renderiza un PNG con alpha.

Inspirado en Nostale y Character Select screens 2.5D.

---

## Architecture

```
ACharacterBase
├── USkeletalMeshComponent*        (invisible, solo bones + sockets)
├── UCharacterVisualComponent*     (maneja la creación y updates de los planos)
│   ├── TMap<EBodySlotType, UStaticMeshComponent*>     BodyPlanes
│   └── TMap<EEquipmentSlotType, UStaticMeshComponent*> EquipmentPlanes
│
├── UPROPERTY(Replicated) TMap<EEquipmentSlotType, FName> EquippedItems
│   (Solo EquipmentID — cliente resuelve DataAssets)
│
└── UPROPERTY(Replicated) FName CurrentBodyID
    (ID del BodyDataAsset activo — permite swap de body type)
```

---

## Data Layer

### UBodyDataAsset
Define la configuración visual del cuerpo base.

```
/Game/Character/Data/Body/
  DA_Body_Female_Base   (UBodyDataAsset)
  DA_Body_Male_Base     (futuro)
  DA_Body_Orc_Base      (futuro)
```

Campos:
- `FName BodyID` — identificador único
- `TMap<EBodySlotType, FBodySlotData>` Slots — qué plano existe, en qué socket, con qué tamaño
- `UMaterialInterface* BodyMaterial` — material template para body (el mismo para todos los slots)

### UEquipmentDataAsset
Define un ítem de equipo.

```
/Game/Equipment/Data/
  E_Gloves_Leather
  E_Boots_Iron
  E_Weapon_Sword_Iron
  ...
```

Campos:
- `FName EquipmentID` — identificador único
- `EEquipmentSlotType SlotType` — a qué slot pertenece
- `FEquipmentSlotData SlotConfig` — socket, offset, tamaño
- `UMaterialInterface* EquipmentMaterial` — material template con la textura PNG
- `TArray<EBodySlotType> HiddenBodySlots` — qué body parts oculta al equipar

---

## Enums

```cpp
// Partes del cuerpo base
UENUM(BlueprintType)
enum class EBodySlotType : uint8
{
    Head,
    Torso,
    Pelvis,
    LeftUpperArm,
    RightUpperArm,
    LeftLowerArm,
    RightLowerArm,
    LeftHand,
    RightHand,
    LeftThigh,
    RightThigh,
    LeftLeg,
    RightLeg,
    LeftFoot,
    RightFoot
};

// Slots de equipo
UENUM(BlueprintType)
enum class EEquipmentSlotType : uint8
{
    Head,
    Armor,
    Weapon,
    Boots,
    Gloves,
    Collar,
    Ring
};
```

---

## Structs

### FCharacterSlotConfig (base)
```cpp
USTRUCT(BlueprintType)
struct FCharacterSlotConfig
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName SocketName;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FVector RelativeLocation = FVector::ZeroVector;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FRotator RelativeRotation = FRotator::ZeroRotator;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FVector2D PlaneSize = FVector2D(100.f, 100.f);
};
```

### FBodySlotData
```cpp
USTRUCT(BlueprintType)
struct FBodySlotData : public FCharacterSlotConfig
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly) EBodySlotType SlotType;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FVector2D ZOrder = FVector2D(0.f, 0.f); // sorting layer
};
```

### FEquipmentSlotData
```cpp
USTRUCT(BlueprintType)
struct FEquipmentSlotData : public FCharacterSlotConfig
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<EBodySlotType> HiddenBodySlots;
};
```

---

## Replication Flow

```
[Server] Equip(EquipmentID)
  → Validates slot + permissions
  → EquippedItems.Add(SlotType, EquipmentID)
  → Mark Actor for NetUpdate

[Client] OnRep_EquippedItems
  → For each changed slot:
      If new equipment:
        Load UEquipmentDataAsset by ID
        Set material on EquipmentPlane
        Hide BodySlots listed in HiddenBodySlots
      If unequipped:
        Clear material
        Reveal previously hidden BodySlots
```

El servidor solo envía `FName EquipmentID`. El cliente resuelve el `UEquipmentDataAsset` localmente y extrae toda la info visual. Si no encuentra el asset, usa un fallback material.

---

## Material

`M_CharacterPlane_Base` (Material Instance Constant padre):
- `TextureParameter` "BaseColorTex" — la textura PNG del layer
- `Blend Mode` → **Translucent**
- `Two Sided` → true
- `TextureGroup` → `Character`
- `Shading Model` → `Unlit` o `Default Lit` según look deseado

En runtime, `UCharacterVisualComponent` crea un `UMaterialInstanceDynamic` por cada slot y hace `SetTextureParameterValue`.

---

## Sorting / Z-Order

Los equipos que van "sobre" el body (capas superiores) necesitan tener un sorting offset mayor.
- Body: z-order base = 0
- Gloves/Boots: z-order = 10
- Armor: z-order = 20
- Weapon: z-order = 30
- Cape/Accessory: z-order = 40

Esto se configura en el `FBodySlotData.ZOrder` y `FEquipmentSlotData.ZOrder`.

---

## File Structure

```
Source/PlatformGame/
├── Character/
│   ├── ECharacterSlots.h
│   ├── CharacterSlotStructs.h
│   ├── CharacterVisualComponent.h / .cpp
│   ├── CharacterBase.h / .cpp
│   └── Data/
│       ├── EquipmentDataAsset.h / .cpp
│       └── BodyDataAsset.h / .cpp

Content/
├── Character/
│   ├── Materials/
│   │   └── M_CharacterPlane_Base.uasset
│   └── Data/
│       ├── Body/
│       │   └── DA_Body_Female_Base.uasset
│       └── Equipment/
│           ├── E_Gloves_Leather.uasset
│           └── E_Boots_Iron.uasset
```

---

## Scope de esta implementación (v1)

- ✅ Enums y structs
- ✅ UEquipmentDataAsset + UBodyDataAsset
- ✅ UCharacterVisualComponent (crea/actualiza/destruye los planos)
- ✅ ACharacterBase (equip/unequip, replication)
- ✅ Material base M_CharacterPlane_Base
- ✅ SkeletalMesh placeholder (mesh vacío, con sockets definidos)
- ❌ Sistema de inventory (futuro)
- ❌ Sistema de reliquias/tarjetas especialista (futuro)
- ❌ Animación 3D del rig (futuro — por ahora idle)

---

## Decisiones tomadas

1. **Dynamic Material Instances** por slot — permite swap de material sin recrear el plane
2. **Hide system via HiddenBodySlots array** — simple y extensible
3. **Réplica solo por EquipmentID** — mínimo bandwidth, máximo flexibilidad
4. **Body swap via BodyID** — soporta múltiples body types sin cambiar código
5. **Socket-based attachment** — los planos se attachan a sockets del SkeletalMesh, no a bones directamente (más flexible para animator)

---

## Next Steps después de v1

1. Importar el SkeletalMesh con los sockets necesarios
2. Configurar los DA_Body y DA_Equipment con las texturas reales
3. Hook con el sistema de stats del backend
4. Sistema de animación idle/walk/attack basado en los bones del rig
5. Sistema de reliquias/tarjetas especialista
