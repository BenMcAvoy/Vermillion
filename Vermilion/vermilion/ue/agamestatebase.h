#pragma once

namespace Vermilion::UE {
	class AGameStateBase : public AActor {
	public:
		VERMILION_PROP(AGameStateBase, TArray<APlayerState*>, PlayerArray, _PlayerArray)

	private:
		/* 0x2E0 */ char pad_0x2E0[0x38];
		/* 0x318 */ TArray<APlayerState*> _PlayerArray;
	};
} // namespace Vermilion::UE
