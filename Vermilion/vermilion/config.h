#pragma once

#define VERMILLION_FARLIGHT84

namespace Vermilion {
#ifdef VERMILLION_FARLIGHT84
	constexpr auto ProcessName = L"SolarlandClient-Win64-Shipping.exe";
#else
#error "Currently only Farlight84 is supported"
#endif

	constexpr auto DriverPath = L"\\\\.\\colonelLink";

	constexpr ImColor AccentColour{ 227, 66, 52 };
} // namespace Vermilion
