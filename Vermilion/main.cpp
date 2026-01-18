#define VERMILLION_FARLIGHT84

#include <vermilion/sdk.h>

void entry() {
    using namespace Vermilion;

    UE::Init();

    UE::FName fWorldName = UE::GWorld->NamePrivate;
    std::string& worldName = fWorldName.ToString();
    std::println("World name: {}", worldName);
}

int main() {
    try {
        entry();
    } catch (const std::exception& e) {
        std::println("Error: {}", e.what());
	}

    return 0;
}
