#pragma once

namespace Vermilion::UE {
	class APlayerState : public AActor {
	public:
		VERMILION_PROP(APlayerState, APawn*, PawnPrivate, _PawnPrivate)

	private:
		/* 0x2E0 */ char pad_0x2E0[0x68];
		/* 0x348 */ APawn* _PawnPrivate;
	};
} // namespace Vermilion::UE
