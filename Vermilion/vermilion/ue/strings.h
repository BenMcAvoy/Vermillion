#pragma once

#include "vermilion/driver.h"

namespace Vermilion::UE {
	static inline class FNamePool* GNamePool = nullptr;

	struct FName;

	template <typename T>
	concept FNameOrUint32 =
		std::same_as<std::remove_cvref_t<T>, uint32_t> ||
		std::is_same_v<std::remove_cvref_t<T>, FName>;

	// TODO: GObject iteration to resolve all FNames at startup
	class FNamePool {
	public:
		static FNamePool* Get() {
			uintptr_t poFNamePool = Vermilion::GDriver.GetBaseAddress() + Vermilion::Offsets::GNamePool_Offset;
			FNamePool* pFNamePool = reinterpret_cast<FNamePool*>(poFNamePool);
			GNamePool = pFNamePool;
			return pFNamePool;
		}

		template <FNameOrUint32 T>
		std::string* GetNameAsString(T fNameOrUint32) const {
			uint32_t ComparisonIdx = 0;

			using U = std::remove_cvref_t<T>;
			if constexpr (std::same_as<U, FName>) {
				ComparisonIdx = fNameOrUint32.ComparisonIndex;
			}
			else if constexpr (std::same_as<U, uint32_t>) {
				ComparisonIdx = fNameOrUint32;
			}

			auto it = nameCache_.find(ComparisonIdx);
			if (it != nameCache_.end()) {
				std::optional<std::string>& val = it->second;
				return val ? &val.value() : nullptr;
			}

			uint32_t chunkOffset = ComparisonIdx >> 16;
            uint16_t nameOffset = static_cast<uint16_t>(ComparisonIdx & 0xFFFF);

			auto oName = _GetNameAsString(chunkOffset, nameOffset);
			nameCache_[ComparisonIdx] = oName;

			std::optional<std::string>& val = nameCache_[ComparisonIdx];
			return val ? &val.value() : nullptr;
		}

	private:
		std::optional<std::string> _GetNameAsString(uint32_t chunkOffset, uint16_t nameOffset) const {
			uintptr_t pThis = reinterpret_cast<uintptr_t>(this);
			uintptr_t pChunk = GDriver.Read<uintptr_t>(pThis + 8ull * (chunkOffset + 2));
			uintptr_t entryOffset = pChunk + 2ull * nameOffset;

			uint16_t nameLength = (GDriver.Read<uint16_t>(entryOffset)) >> 6;

			if (nameLength == 0 || nameLength > 1024) {
				return std::nullopt;
			}

			std::string nameBuffer(nameLength, '\0');
			auto oNameBuffer = reinterpret_cast<void*>(nameBuffer.data());
			GDriver.Read(entryOffset + 2, oNameBuffer, nameLength);
			return nameBuffer;
		}

	private:
		static inline std::unordered_map<uint32_t, std::optional<std::string>> nameCache_;
	};

	struct FName {
		uint32_t ComparisonIndex;
		uint32_t Number; // ?

		FName() : ComparisonIndex(0), Number(0) {}
		FName(uint32_t _ComparisonIndex) : ComparisonIndex(_ComparisonIndex), Number(0) {}
		FName(uint32_t _ComparisonIndex, uint32_t _Number) : ComparisonIndex(_ComparisonIndex), Number(_Number) {}

		std::string& ToString() const {
			auto pStr = GNamePool->GetNameAsString(ComparisonIndex);
			static std::string invalid("<INVALID FNAME>");
			return *(pStr ? pStr : &invalid);
		}
	};
} // namespace Vermilion::UE