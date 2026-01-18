#pragma once

namespace Vermilion::UE {
	static inline class UWorld* GWorld = nullptr;

	class UWorld : public UObject {
	public:
		static UWorld* Get() {
			uintptr_t poUWorld = Vermilion::GDriver.GetBaseAddress() + Vermilion::Offsets::GWorld_Offset;
			UWorld* pUWorld = GDriver.Read<UWorld*>(poUWorld);
			GWorld = pUWorld;
			return pUWorld;
		}

		VERMILION_PROP(UWorld, UGameInstance*, OwningGameInstance, _OwningGameInstance);
		VERMILION_PROP(UWorld, AGameStateBase*, GameState, _GameState);

	private:
		/* 0x028 */ char pad_0x028[0x178];
		/* 0x1A0 */ class AGameStateBase* _GameState;
		/* 0x1A8 */ char pad_0x1A8[0x50];
		/* 0x1F8 */ class UGameInstance* _OwningGameInstance;
		/* 0x200 */ char pad_0x1F0[0x660];
	};

	VERMILION_ASSERT_SIZE(UWorld, 0x860);
} // namespace Vermilion::UE
