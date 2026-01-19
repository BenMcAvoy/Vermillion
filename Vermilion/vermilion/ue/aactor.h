#pragma once

namespace Vermilion::UE {
	class AActor : public UObject {
	public:
		VERMILION_PROP(AActor, class USceneComponent*, RootComponent, _RootComponent)

	private:
		/* 0x028 */ char pad_0x028[0x188];
		/* 0x1B0 */ class USceneComponent* _RootComponent;
		/* 0x1B8 */ char pad_0x1B8[0x128];
	};

	static_assert(sizeof(AActor) == 0x2E0, "AActor needs to be 0x2E0 bytes");
}
