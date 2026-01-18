#pragma once

namespace Vermilion::UE {
    struct FMeshBoneInfo {
        uint32_t NameIdx;
        int32_t ParentIdx;
        char pad_0x000[0x4];
    };

    struct FReferenceSkeleton {
        /* 0x000 */ TArray<FMeshBoneInfo> RawRefBoneInfo;
        /* 0x010 */ TArray<FTransform> RawRefBonePos;

        /* 0x020 */ TArray<FMeshBoneInfo> FinalRefBoneInfo;
        /* 0x030 */ TArray<FTransform> FinalRefBonePos;

        std::optional<int> GetBoneIndexByName(std::string_view boneName) {
            try {
                auto& boneInfos = RawRefBoneInfo;

                size_t index = 0;
                for (auto boneInfo : boneInfos) {
                    try {
                        FName name(boneInfo.NameIdx);
                        std::string& nameStr = name.ToString();

                        if (nameStr == boneName) {
                            return index;
                        }

                        index++;
                    }
                    catch (std::exception) {
                        std::string msg = std::format(
                            "Failed to get bone name at index {}: {}",
                            index,
                            boneInfo.NameIdx
						);
                        index++;
                        continue;
                    }
                }

                return std::nullopt;
            }
            catch (std::exception) {
                return {};
            }
        }
    };

    // Technically inherits from UStreamableRenderAsset
	class USkeletalMesh : public UObject {
    public:
        VERMILION_PROP(USkeletalMesh, FReferenceSkeleton, RefSkeleton, _RefSkeleton)

    private:
		/* 0x0028 */ char pad_0x0028[0x190];
		/* 0x01B8 */ FReferenceSkeleton _RefSkeleton;
    };

    enum class EVisibilityBasedAnimTickOption : uint8_t {
        AlwaysTickPoseAndRefreshBones = 0,
        AlwaysTickPose = 1,
        OnlyTickMontagesWhenNotRendered = 2,
        OnlyTickPoseWhenRendered = 3,
        EVisibilityBasedAnimTickOption_MAX = 4
    };

	class UPrimitiveComponent : public USceneComponent {};
	class UMeshComponent : public UPrimitiveComponent {};
    class USkinnedMeshComponent : public UMeshComponent {
    public:
		VERMILION_PROP(USkinnedMeshComponent, USkeletalMesh*, SkeletalMesh, _SkeletalMesh)

		__declspec(property(get = _getBoneArray)) TArray<FTransform> BoneArray;

        TArray<FTransform> _getBoneArray() {
			TArray<FTransform> transforms1 = CachedComponentSpaceTransforms1;
			return (transforms1.Count == 0) ? CachedComponentSpaceTransforms2 : transforms1;
		}

        VERMILION_PROP(USkinnedMeshComponent, TArray<FTransform>, CachedComponentSpaceTransforms1, _CachedComponentSpaceTransforms1);
        VERMILION_PROP(USkinnedMeshComponent, TArray<FTransform>, CachedComponentSpaceTransforms2, _CachedComponentSpaceTransforms2);

		VERMILION_PROP(USkinnedMeshComponent, uint8_t, Flags, _Flags);

		VERMILION_PROP(USkinnedMeshComponent, EVisibilityBasedAnimTickOption, VisibilityBasedAnimTickOption, _VisibilityBasedAnimTickOption);

    private:
		/* 0x0158 */ char pad_0x0158[0x5A8];
		/* 0x0700 */ USkeletalMesh* _SkeletalMesh;
		/* 0x0708 */ char pad_0x0708[0x18];
        /* 0x0720 */ TArray<FTransform> _CachedComponentSpaceTransforms1;
        /* 0x0730 */ TArray<FTransform> _CachedComponentSpaceTransforms2;
		/* 0x0740 */ char pad_0x0740[0x134];
        /* 0x0874 */ EVisibilityBasedAnimTickOption _VisibilityBasedAnimTickOption;
        /* 0x0878 */ uint8_t _Flags;
    };

    class USkeletalMeshComponent : public USkinnedMeshComponent { };
} // namespace Vermilion::UE
