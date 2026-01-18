#pragma once

namespace Vermilion::UE {
    class UActorComponent : public UObject {};

    class USceneComponent : public UActorComponent {
    public:
        VERMILION_PROP(USceneComponent, FVector, RelativeLocation, _RelativeLocation)
        VERMILION_PROP_OFFSET(USceneComponent, FTransform, ComponentToWorld, 0x280);

    private:
        char pad_0x0028[0x124];
        FVector _RelativeLocation;
    };
};
