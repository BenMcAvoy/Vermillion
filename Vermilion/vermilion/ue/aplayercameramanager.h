#pragma once

namespace Vermilion::UE {
    struct FMinimalViewInfo {
        /* 0x0000 */ FVector Location;
        /* 0x000C */ FRotator Rotation;
    private:
        /* 0x0018 */ char pad_0x0018[0x68];
    public:
        /* 0x0080 */ float FOV;
        /* 0x0084 */ float DesiredFOV;
        /* 0x0088 */ float FirstPersonFOV;
        /* 0x008C */ float OrthoWidth;
        /* 0x0090 */ float OrthoNearClipPlane;
        /* 0x0094 */ float OrthoFarClipPlane;
        /* 0x0098 */ float AspectRatio;

        std::string ToString() {
            return std::format("FMinimalViewInfo(Location: {}, Rotation: {}, FOV: {}, DesiredFOV: {}, FirstPersonFOV: {}, OrthoWidth: {}, OrthoNearClipPlane: {}, OrthoFarClipPlane: {}, AspectRatio: {})",
                Location.ToString(), Rotation.ToString(), FOV, DesiredFOV, FirstPersonFOV, OrthoWidth, OrthoNearClipPlane, OrthoFarClipPlane, AspectRatio);
        }
    };

    struct FCameraCacheEntry {
        /* 0x0000 */ float Timestamp;
    private:
        /* 0x0004 */ char pad_0x0004[0xC];
    public:
        /* 0x0010 */ FMinimalViewInfo POV;

        std::string ToString() {
            return std::format("FCameraCacheEntry(Timestamp: {}, POV: {})", Timestamp, POV.ToString());
        }
    };

    class APlayerCameraManager : public AActor {
    public:
        VERMILION_PROP(APlayerCameraManager, FCameraCacheEntry, CameraCachePrivate, _CameraCachePrivate)

        std::optional<FVector2D> WorldToScreen(
            const FVector& WorldPos,
            uint32_t Width,
            uint32_t Height
        ) const {
            if (!this) return std::nullopt;

            auto POV = CameraCachePrivate.POV;

            FMatrix View(POV.Rotation);

            const FVector VAxisX{
                View.M[0][0], View.M[0][1], View.M[0][2]
            };
            const FVector VAxisY{
                View.M[1][0], View.M[1][1], View.M[1][2]
            };
            const FVector VAxisZ{
                View.M[2][0], View.M[2][1], View.M[2][2]
            };

            FVector Delta = WorldPos - POV.Location;

            const FVector Transformed{
                Delta.Dot(VAxisY),
                Delta.Dot(VAxisZ),
                Delta.Dot(VAxisX)
            };

            if (Transformed.Z < 1.0f)
                return std::nullopt;

            const float ScreenCenterX = Width * 0.5f;
            const float ScreenCenterY = Height * 0.5f;

            const float D =
                ScreenCenterY /
                std::tan((POV.FOV * 0.5f) * 3.14159265f / 180.0f);

            const float FOV =
                2.0f * std::atan(ScreenCenterX / D) * 180.0f / 3.14159265f;

            const float FOVRad = FOV * 3.14159265f / 360.0f;
            const float TanFOV = std::tan(FOVRad);

            FVector2D Screen;
            Screen.X =
                ScreenCenterX +
                (Transformed.X * (ScreenCenterX / TanFOV)) / Transformed.Z;

            Screen.Y =
                ScreenCenterY -
                (Transformed.Y * (ScreenCenterX / TanFOV)) / Transformed.Z;

            return Screen;
        }

    private:
        /* 0x02E0 */ char pad_0x02E0[0x2B80];
		/* 0x2E60 */ FCameraCacheEntry _CameraCachePrivate;
		/* 0x3580 */ char pad_0x3580[0x26E0];
    };
} // namespace Vermilion::UE
