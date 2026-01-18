#pragma once

namespace Vermilion::UE {
	class APlayerController : public AActor {
	public:
		VERMILION_PROP(APlayerController, APawn*, AcknowledgedPawn, _AcknowledgedPawn);
		VERMILION_PROP(APlayerController, APlayerCameraManager*, PlayerCameraManager, _PlayerCameraManager);

	private:
		/* 0x2E0 */ char pad_0x2E0[0x98];
		/* 0x378 */ APawn* _AcknowledgedPawn;
		/* 0x380 */ char pad_0x380[0x10];
		/* 0x390 */ APlayerCameraManager* _PlayerCameraManager;
		/* 0x398 */ char pad_0x398[0x300];
	};

	VERMILION_ASSERT_SIZE(APlayerController, 0x698);
}
