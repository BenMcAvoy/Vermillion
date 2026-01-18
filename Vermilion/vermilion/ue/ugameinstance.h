#pragma once

namespace Vermilion::UE {
	class UGameInstance : public UObject {
	public:
		VERMILION_PROP(UGameInstance, TArray<ULocalPlayer*>, LocalPlayers, _LocalPlayers);

	private:
		/* 0x028 */ char pad_0x028[0x10];
		/* 0x038 */ TArray<class ULocalPlayer*> _LocalPlayers;
		/* 0x048 */ char pad_0x048[0x158];
	};

	VERMILION_ASSERT_SIZE(UGameInstance, 0x1A0);
}
