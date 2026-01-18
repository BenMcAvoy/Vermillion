#pragma once

namespace Vermilion::UE {
	class ULocalPlayer : public UPlayer {
		char pad_0x48[0x210];
	};

	VERMILION_ASSERT_SIZE(ULocalPlayer, 0x258);
} // namespace Vermilion::UE
