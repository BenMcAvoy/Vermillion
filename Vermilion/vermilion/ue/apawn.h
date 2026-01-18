#pragma once

namespace Vermilion::UE {
	class APawn : public AActor {
		/* 0x2E0 */ char pad_0x2E0[0x60];
	};

	VERMILION_ASSERT_SIZE(APawn, 0x340);
} // namespace Vermilion::UE
