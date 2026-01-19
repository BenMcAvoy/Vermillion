#define VERMILLION_FARLIGHT84

#define NOMINMAX

#include <vermilion/render.h>
#include <vermilion/sdk.h>

#include <span>

#define RENDERCONTINUE {Render.EndFrame(); continue; }

using namespace Vermilion;

std::unordered_map<UE::USkeletalMesh*, std::vector<std::pair<int, int>>> g_pairCache;
std::span<std::pair<int, int>> GetSkeletonBonePairs(UE::USkeletalMesh* skelMesh) {
    if (g_pairCache.find(skelMesh) != g_pairCache.end()) {
        auto& cachedPairs = g_pairCache[skelMesh];
        return { cachedPairs.data(), cachedPairs.size() };
    }

    std::vector<std::pair<int, int>> bonePairs;
    if (!skelMesh) return {};

    auto refSkel = skelMesh->RefSkeleton;

    std::pair<const char*, const char*> SkeletonPairs[] = {
        // Spine
        {"pelvis",        "spine_01"},
        {"spine_01",      "spine_02"},
        {"spine_02",      "spine_03"},
        {"spine_03",      "neck_01"},

        // Left arm
        {"neck_01",     "clavicle_l"},
        {"clavicle_l",  "upperarm_l"},
        {"upperarm_l",  "lowerarm_l"},
        {"lowerarm_l",  "hand_l"},

        // Right arm
        {"neck_01",     "clavicle_r"},
        {"clavicle_r",  "upperarm_r"},
        {"upperarm_r",  "lowerarm_r"},
        {"lowerarm_r",  "hand_r"},

        // Left leg
        {"pelvis",        "thigh_l"},
        {"thigh_l",       "calf_l"},
        {"calf_l",        "foot_l"},
        {"foot_l",        "ball_l"},

        // Right leg
        {"pelvis",        "thigh_r"},
        {"thigh_r",       "calf_r"},
        {"calf_r",        "foot_r"},
        {"foot_r",        "ball_r"},
    };

    for (auto& boneNamePair : SkeletonPairs) {
        auto boneAIndex = refSkel.GetBoneIndexByName(boneNamePair.first);
        auto boneBIndex = refSkel.GetBoneIndexByName(boneNamePair.second);

        if (boneAIndex && boneBIndex) {
            int boneAIndexR = *boneAIndex;
            int boneBIndexR = *boneBIndex;

            bonePairs.emplace_back(boneAIndexR, boneBIndexR);
        }
    }

    g_pairCache[skelMesh] = bonePairs;

    return {
        g_pairCache[skelMesh].data(),
        g_pairCache[skelMesh].size(),
    };
}

void watermark() {
    ImDrawList* draw = ImGui::GetForegroundDrawList();
    ImGuiIO& io = ImGui::GetIO();

    const char* title = "Vermilion";
    const char* sep   = "|";

    char fpsBuf[32];
    snprintf(fpsBuf, sizeof(fpsBuf), "%.0f FPS", io.Framerate);

    ImVec2 padding(8.0f, 6.0f);
    float accentWidth = 3.0f;

    ImVec2 titleSize = ImGui::CalcTextSize(title);
    ImVec2 sepSize   = ImGui::CalcTextSize(sep);
    ImVec2 fpsSize   = ImGui::CalcTextSize(fpsBuf);

    float textHeight = std::max({
        titleSize.y,
        sepSize.y,
        fpsSize.y
    });

    float height =
        padding.y * 2.0f +
        textHeight;

    float width =
        accentWidth +
        padding.x +
        titleSize.x +
        padding.x * 0.5f +
        sepSize.x +
        padding.x * 0.5f +
        fpsSize.x +
        padding.x;

    ImVec2 pos(10.0f, 10.0f);
    ImVec2 end(pos.x + width, pos.y + height);

    ImU32 bgCol     = IM_COL32(20, 20, 20, 220);
    ImU32 accentCol = Vermilion::AccentColour;
    ImU32 textCol   = IM_COL32(230, 230, 230, 255);
    ImU32 fpsCol    = IM_COL32(160, 160, 160, 255);

    draw->AddRectFilled(pos, end, bgCol, 4.0f);

    draw->AddRectFilled(
        pos,
        ImVec2(pos.x + accentWidth, end.y),
        accentCol,
        4.0f,
        ImDrawFlags_RoundCornersLeft
    );

    float textY = pos.y + (height - textHeight) * 0.5f;
    float x = pos.x + accentWidth + padding.x;

    draw->AddText(ImVec2(x, textY), accentCol, title);
    x += titleSize.x + padding.x * 0.5f;

    draw->AddText(ImVec2(x, textY), textCol, sep);
    x += sepSize.x + padding.x * 0.5f;

    draw->AddText(ImVec2(x, textY), fpsCol, fpsBuf);
}

void entry() {
    UE::Init();

    /*auto localPlayer = UE::GWorld
		->OwningGameInstance
		->LocalPlayers[0]
		->PlayerController;

	auto localPawn = localPlayer->AcknowledgedPawn;*/

    auto localPlayer = UE::GWorld
        ->OwningGameInstance
        ->LocalPlayers[0];

	auto gameState = UE::GWorld->GameState;

    if (!localPlayer || !gameState) {
		std::println("Failed to get local player or game state.");
        return;
    }
 
    Render.Init();

    while (true) {
        Render.BeginFrame();

        watermark();

        if (!localPlayer) RENDERCONTINUE;
		auto localController = localPlayer->PlayerController;
        if (!localController) RENDERCONTINUE;

        auto players = gameState->PlayerArray;
		if (players.Count == 0 || players.Count > 1000) RENDERCONTINUE;

        for (auto player : players) {
            auto playerPawn = player->PawnPrivate;
            if (!playerPawn) continue;

			UE::ASolarPlayState* solarPlayerState = static_cast<UE::ASolarPlayState*>(player);
			if (!solarPlayerState) continue;

            if (solarPlayerState->CurrentCharacterStateInGame == 4) // Dead
				continue;

            if (localController->AcknowledgedPawn == playerPawn)
                continue;

            auto playerChar = static_cast<UE::ACharacter*>(playerPawn);
            if (!playerChar) continue;
			if (!playerChar->Mesh) continue;
			if (!playerChar->Mesh->SkeletalMesh) continue;

            // Draw tracer
            auto location = playerChar->RootComponent->RelativeLocation;
			auto screenPos = localController->PlayerCameraManager->WorldToScreen(location, 1920, 1080);
            if (screenPos) {
                ImGui::GetBackgroundDrawList()->AddLine(
                    ImVec2(1920.0f / 2.0f, 0.0f),
                    screenPos->ToImVec2(),
                    AccentColour,
                    0.2f
                    );
            }

            auto bonePairs = GetSkeletonBonePairs(playerChar->Mesh->SkeletalMesh);
            UE::FTransform c2w = playerChar->Mesh->ComponentToWorld;
            UE::FMatrix c2wMatrix = c2w.ToMatrix();

            auto boneArray = playerChar->Mesh->BoneArray;
            for (auto [a, b] : bonePairs) {
                if (boneArray.Count == 0) continue;

                UE::FMatrix jointA, jointB;
                try {
                    jointA = boneArray[a].ToMatrix();
                    jointB = boneArray[b].ToMatrix();
                } catch (std::exception) {
                    continue;
				}

                auto jointAWorld = jointA * c2wMatrix;
                auto jointBWorld = jointB * c2wMatrix;

                auto jointAFVec = UE::FVector{
                    jointAWorld.M[3][0],
                    jointAWorld.M[3][1],
                    jointAWorld.M[3][2],
                };

                auto jointBFVec = UE::FVector{
                    jointBWorld.M[3][0],
                    jointBWorld.M[3][1],
                    jointBWorld.M[3][2],
                };

                auto jointAScreenPos = localController->PlayerCameraManager->WorldToScreen(jointAFVec, 1920, 1080);
                auto jointBScreenPos = localController->PlayerCameraManager->WorldToScreen(jointBFVec, 1920, 1080);

                if (jointAScreenPos && jointBScreenPos) {
                    ImGui::GetBackgroundDrawList()->AddLine(
                        jointAScreenPos->ToImVec2(),
                        jointBScreenPos->ToImVec2(),
						AccentColour,
                        2.0f
                    );
                }
            }
        }

end:
        Render.EndFrame();
    }
}

int main() {
    entry();

    return 0;
}
