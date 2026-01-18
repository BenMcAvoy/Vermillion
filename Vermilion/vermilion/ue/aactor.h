#pragma once

namespace Vermilion::UE {
	class AActor : public UObject {
		/* 0x028 */ char pad_0x028[0x2B8];
	};

	static_assert(sizeof(AActor) == 0x2E0, "AActor needs to be 0x2E0 bytes");
}
