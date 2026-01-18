#pragma once

namespace Vermilion::UE {
	class UPlayer : UObject {
	public:
		VERMILION_PROP(UPlayer, APlayerController*, PlayerController, _PlayerController);

	private:
		/* 0x028 */ char pad_0x028[0x8];
		/* 0x030 */ APlayerController* _PlayerController;
		/* 0x038 */ char pad_0x038[0x10];
	};

	VERMILION_ASSERT_SIZE(UPlayer, 0x48);
} // namespace Vermilion::UE
