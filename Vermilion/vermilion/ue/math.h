#pragma once

namespace Vermilion::UE {
    struct FQuat {
        float X;
        float Y;
        float Z;
        float W;

        FQuat() : X(0), Y(0), Z(0), W(1) {}
        FQuat(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {}

        std::string ToString() const {
            return std::format("FQuat(X: {}, Y: {}, Z: {}, W: {})", X, Y, Z, W);
        }
    };

    struct FVector {
        float X;
        float Y;
        float Z;

        FVector() : X(0), Y(0), Z(0) {}
        FVector(float x, float y, float z) : X(x), Y(y), Z(z) {}

        FVector operator+(const FVector& other) const {
            return FVector(X + other.X, Y + other.Y, Z + other.Z);
        }
        FVector operator-(const FVector& other) const {
            return FVector(X - other.X, Y - other.Y, Z - other.Z);
        }
        FVector operator*(float scalar) const {
            return FVector(X * scalar, Y * scalar, Z * scalar);
        }
        FVector operator/(float scalar) const {
            return FVector(X / scalar, Y / scalar, Z / scalar);
        }

        float Size() const {
            return std::sqrt(X * X + Y * Y + Z * Z);
        }

        float Dot(FVector Other) {
            return X * Other.X + Y * Other.Y + Z * Other.Z;
        }

        std::string ToString() const {
            return std::format("FVector(X: {}, Y: {}, Z: {})", X, Y, Z);
        }
    };

    struct FRotator {
        float Pitch;
        float Yaw;
        float Roll;

        FRotator() : Pitch(0), Yaw(0), Roll(0) {}
        FRotator(float pitch, float yaw, float roll) : Pitch(pitch), Yaw(yaw), Roll(roll) {}

        std::string ToString() const {
            return std::format("FRotator(Pitch: {}, Yaw: {}, Roll: {})", Pitch, Yaw, Roll);
        }
    };

    struct FMatrix {
        float M[4][4] = {
            { 0.0, 0.0, 0.0, 0.0 },
            { 0.0, 0.0, 0.0, 0.0 },
            { 0.0, 0.0, 0.0, 0.0 },
            { 0.0, 0.0, 0.0, 0.0 },
        };

        FMatrix() {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    M[i][j] = 0.0f;
                }
            }
        }
        explicit FMatrix(const FRotator& Rot) {
            float RadPitch = Rot.Pitch * 3.14159265f / 180.0f;
            float RadYaw = Rot.Yaw * 3.14159265f / 180.0f;
            float RadRoll = Rot.Roll * 3.14159265f / 180.0f;

            const float SP = std::sin(RadPitch);
            const float CP = std::cos(RadPitch);
            const float SY = std::sin(RadYaw);
            const float CY = std::cos(RadYaw);
            const float SR = std::sin(RadRoll);
            const float CR = std::cos(RadRoll);

            M[0][0] = CP * CY;
            M[0][1] = CP * SY;
            M[0][2] = SP;
            M[0][3] = 0.0f;

            M[1][0] = SR * SP * CY - CR * SY;
            M[1][1] = SR * SP * SY + CR * CY;
            M[1][2] = -SR * CP;
            M[1][3] = 0.0f;

            M[2][0] = -(CR * SP * CY + SR * SY);
            M[2][1] = CY * SR - CR * SP * SY;
            M[2][2] = CR * CP;
            M[2][3] = 0.0f;

            M[3][0] = 0.0f; // origin.roll
            M[3][1] = 0.0f; // origin.yaw
            M[3][2] = 0.0f; // origin.pitch
            M[3][3] = 1.0f;
        }

        FMatrix operator*(const FMatrix& Other) const {
            FMatrix Result;

            for (int row = 0; row < 4; ++row) {
                for (int col = 0; col < 4; ++col) {
                    Result.M[row][col] =
                        M[row][0] * Other.M[0][col] +
                        M[row][1] * Other.M[1][col] +
                        M[row][2] * Other.M[2][col] +
                        M[row][3] * Other.M[3][col];
                }
            }

            return Result;
        }

        std::string ToString() const {
            std::string result = "FMatrix(\n";
            for (int i = 0; i < 4; ++i) {
                result += "  [";
                for (int j = 0; j < 4; ++j) {
                    result += std::format("{:>8.3f}", M[i][j]);
                    if (j < 3) result += ", ";
                }
                result += "]\n";
            }
            result += ")";
            return result;
        }
    };

    struct FTransform {
        FQuat Rotation;
        FVector Translation;
        char pad_0x01C[0x4];
        FVector Scale3D;
        char pad_0x02C[0x4];

        FMatrix ToMatrix() const {
            FMatrix matrix{};

            matrix.M[3][0] = Translation.X;
            matrix.M[3][1] = Translation.Y;
            matrix.M[3][2] = Translation.Z;

            const float x2 = Rotation.X + Rotation.X;
            const float y2 = Rotation.Y + Rotation.Y;
            const float z2 = Rotation.Z + Rotation.Z;

            const float xx2 = Rotation.X * x2;
            const float yy2 = Rotation.Y * y2;
            const float zz2 = Rotation.Z * z2;

            matrix.M[0][0] = (1.0f - (yy2 + zz2)) * Scale3D.X;
            matrix.M[1][1] = (1.0f - (xx2 + zz2)) * Scale3D.Y;
            matrix.M[2][2] = (1.0f - (xx2 + yy2)) * Scale3D.Z;

            const float yz2 = Rotation.Y * z2;
            const float wx2 = Rotation.W * x2;
            matrix.M[2][1] = (yz2 - wx2) * Scale3D.Z;
            matrix.M[1][2] = (yz2 + wx2) * Scale3D.Y;

            const float xy2 = Rotation.X * y2;
            const float wz2 = Rotation.W * z2;
            matrix.M[1][0] = (xy2 - wz2) * Scale3D.X;
            matrix.M[0][1] = (xy2 + wz2) * Scale3D.Y;

            const float xz2 = Rotation.X * z2;
            const float wy2 = Rotation.W * y2;
            matrix.M[2][0] = (xz2 + wy2) * Scale3D.X;
            matrix.M[0][2] = (xz2 - wy2) * Scale3D.Z;

            matrix.M[0][3] = 0.0f;
            matrix.M[1][3] = 0.0f;
            matrix.M[2][3] = 0.0f;
            matrix.M[3][3] = 1.0f;

            return matrix;
        }

        std::string Tostring() const {
            return std::format(
                "FTransform(Rotation: {}, Translation: {}, Scale3D: {})",
                Rotation.ToString(),
                Translation.ToString(),
                Scale3D.ToString()
            );
        }
    };
} // namespace Vermilion::UE
