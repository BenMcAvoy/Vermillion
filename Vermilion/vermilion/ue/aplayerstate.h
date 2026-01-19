#pragma once

namespace Vermilion::UE {
	class APlayerState : public AActor {
	public:
		VERMILION_PROP(APlayerState, APawn*, PawnPrivate, _PawnPrivate)

	private:
		/* 0x2E0 */ char pad_0x2E0[0x68];
		/* 0x348 */ APawn* _PawnPrivate;
		/* 0x350 */ char pad_0x350[0x98];
	};
	VERMILION_ASSERT_SIZE(APlayerState, 0x3E8);
	class AModulerPlayerState : public APlayerState {};
	VERMILION_ASSERT_SIZE(AModulerPlayerState, 0x3E8);

	class ASolarPlayState : public AModulerPlayerState {
	public:
		VERMILION_PROP(ASolarPlayState, FString, NickName, _NickName)
		VERMILION_PROP(ASolarPlayState, uint8_t, CurrentCharacterStateInGame, _CurrentCharacterStateInGame)
		VERMILION_PROP(ASolarPlayState, bool, bIsSameTeamAsLocalPlayer, _bIsSameTeamAsLocalPlayer)

	private:
		/* 0x3E8 */ char pad_0x3E8[0xC0];
		/* 0x4A8 */ FString _NickName;
		/* 0x4B8 */ char pad_0x4B8[0x2C8];
		/* 0x780 */ uint8_t _CurrentCharacterStateInGame;
		/* 0x788 */ char pad_0x788[0xB7];
		/* 0x838 */ bool _bIsSameTeamAsLocalPlayer;
		/* 0x839 */ char pad_0x839[0x1337];
	};

	VERMILION_ASSERT_SIZE(ASolarPlayState, 0x1B70);
} // namespace Vermilion::UE
