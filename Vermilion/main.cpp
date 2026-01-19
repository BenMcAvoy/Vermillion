#define NOMINMAX
#define IMGUI_DEFINE_MATH_OPERATORS

#include <vermilion/render.h>
#include <vermilion/sdk.h>

#include <span>
#include <string>

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

void windowheader(const char* title) {
    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 padding(8.0f, 6.0f);
    float accentWidth = 3.0f;
    ImVec2 titleSize = ImGui::CalcTextSize(title);
    float textHeight = titleSize.y;
    float height =
        padding.y * 2.0f +
        textHeight;
    // width is the full window width
	float width = windowSize.x;
    ImVec2 pos(windowPos.x, windowPos.y);
    ImVec2 end(pos.x + width, pos.y + height);
    ImU32 bgCol     = IM_COL32(20, 20, 20, 220);
    ImU32 accentCol = Vermilion::AccentColour;
    ImU32 textCol   = IM_COL32(230, 230, 230, 255);
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
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + height);
}

bool checkbox(const char* label, bool& v) {
    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImGuiIO& io = ImGui::GetIO();

    // Per-label animation progress (0.0 = unchecked, 1.0 = checked)
    static std::unordered_map<std::string, float> s_progress;

    // Invisible button to capture clicks
    ImGui::InvisibleButton(label, ImVec2(20.0f, 20.0f));
    if (ImGui::IsItemClicked()) {
        v = !v;
    }

    float& prog = s_progress[std::string(label)];
    float target = v ? 1.0f : 0.0f;
    float dt = io.DeltaTime;
    const float speed = 12.0f; // higher = snappier
    float step = std::min(dt * speed, 1.0f);
    prog += (target - prog) * step;

    ImVec2 pos = ImGui::GetItemRectMin();
    ImVec2 end = ImGui::GetItemRectMax();

    // Lerp between frame background and accent colour
    ImVec4 bgCol = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
    ImVec4 accCol = ImGui::ColorConvertU32ToFloat4(Vermilion::AccentColour);
    ImVec4 mixed = ImVec4(
        bgCol.x + (accCol.x - bgCol.x) * prog,
        bgCol.y + (accCol.y - bgCol.y) * prog,
        bgCol.z + (accCol.z - bgCol.z) * prog,
        bgCol.w + (accCol.w - bgCol.w) * prog
    );

    ImU32 boxCol = ImGui::GetColorU32(mixed);
    draw->AddRectFilled(pos, end, boxCol, 4.0f);

    // Animated check: draw partial lines based on progress
    if (prog > 0.001f) {
        ImU32 checkCol = IM_COL32(230, 230, 230, (int)(255 * prog));

        ImVec2 p1 = ImVec2(pos.x + 4.0f, pos.y + 10.0f);
        ImVec2 p2 = ImVec2(pos.x + 8.0f, pos.y + 14.0f);
        ImVec2 p3 = ImVec2(pos.x + 16.0f, pos.y + 6.0f);

        ImVec2 mid1 = ImVec2(
            p1.x + (p2.x - p1.x) * prog,
            p1.y + (p2.y - p1.y) * prog
        );
        ImVec2 mid2 = ImVec2(
            p2.x + (p3.x - p2.x) * prog,
            p2.y + (p3.y - p2.y) * prog
        );

        float thickness = 2.0f * (0.6f + 0.4f * prog);
        draw->AddLine(p1, mid1, checkCol, thickness);
        draw->AddLine(p2, mid2, checkCol, thickness);
    }

    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
    ImGui::TextUnformatted(label);
    return v;
}

void windowdropshadow() {
    // draw a subtle frame around the window
	ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 pos = ImVec2(windowPos.x - 1.0f, windowPos.y - 1.0f);
    ImVec2 end = ImVec2(windowPos.x + windowSize.x + 1.0f, windowPos.y + windowSize.y + 1.0f);
    ImU32 shadowCol = IM_COL32(0, 0, 0, 100);
	draw->AddRect(pos, end, shadowCol, 4.0f);
}

void entry() {
    UE::Init();

    auto localPlayer = UE::GWorld
        ->OwningGameInstance
        ->LocalPlayers[0];

    if (!localPlayer) {
		std::println("Failed to get local player or game state.");
        return;
    }
 
    Render.Init();

    bool showSkeleton   = true;
	bool showTracers    = true;
	bool showTracersTop = true;

    while (true) {
        if (!UE::UWorld::Get())
            continue;

	    auto gameState = UE::GWorld->GameState;
        if (!gameState) continue;

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

            if (solarPlayerState->bIsSameTeamAsLocalPlayer)
				continue;

            if (localController->AcknowledgedPawn == playerPawn)
                continue;

            auto playerChar = static_cast<UE::ACharacter*>(playerPawn);
            if (!playerChar) continue;
			if (!playerChar->Mesh) continue;
			if (!playerChar->Mesh->SkeletalMesh) continue;

            auto bonePairs = GetSkeletonBonePairs(playerChar->Mesh->SkeletalMesh);
            UE::FTransform c2w = playerChar->Mesh->ComponentToWorld;
            UE::FMatrix c2wMatrix = c2w.ToMatrix();

            UE::FVector neckPos;

            auto boneArray = playerChar->Mesh->BoneArray;

            size_t index = 0;
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

				// If index is 3, then Joint A is neck_01
                if (index == 3) {
                    neckPos = jointAFVec;
                    if (!showSkeleton) {
                        break;
					}
				}

                if (!showSkeleton) {
                    index++;
                    continue;
                }

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

				index++;
            }

			// Draw tracer to neck
            if (showTracers) {
                auto neckScreenPos = localController->PlayerCameraManager->WorldToScreen(neckPos, 1920, 1080);
                if (neckScreenPos) {
                    ImGui::GetBackgroundDrawList()->AddLine(
						ImVec2(1920.0f / 2.0f, (showTracersTop ? 0.0f : 1080.0f)),
                        neckScreenPos->ToImVec2(),
                        AccentColour,
                        1.0f
                    );
                }
            }
        }

        if (Render.running) {
			// No titlebar (we render our own)
			ImGui::Begin("Vermilion Menu", nullptr, 
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoResize
			);

			windowheader("Vermilion Menu");
            windowdropshadow();

			checkbox("Show Skeleton", showSkeleton);
			checkbox("Show Tracers", showTracers);
			checkbox("Tracers from Top", showTracersTop);

            ImGui::End();
        }

        Render.EndFrame();
    }
}

int main() {
    entry();

    return 0;
}
