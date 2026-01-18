#pragma once

namespace Vermilion::UE {
	class ACharacter : public APawn {
	public:
		VERMILION_PROP(ACharacter, class USkeletalMeshComponent*, Mesh, _Mesh);

	private:
		/* 0x340 */ USkeletalMeshComponent* _Mesh;
	};
} // namespace Vermilion::UE
