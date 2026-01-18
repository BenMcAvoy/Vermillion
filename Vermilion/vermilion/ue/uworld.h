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

	private:
		char pad_0x028[0x838];
	};

	static_assert(sizeof(UWorld) == 0x860, "UWorld must be 0x860 bytes");
} // namespace Vermilion::UE
