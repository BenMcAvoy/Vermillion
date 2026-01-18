#pragma once

namespace Vermilion::UE {
	class UObject {
	public:
		VERMILLION_PROP(UObject, uintptr_t*, VTable, _VTable);
		VERMILLION_PROP(UObject, uint32_t, ObjectFlags, _ObjectFlags);
		VERMILLION_PROP(UObject, uint32_t, ObjectIndex, _ObjectIndex);
		VERMILLION_PROP(UObject, class UClass*, ClassPrivate, _ClassPrivate);
		__declspec(property(get = _getNamePrivate)) FName NamePrivate; FName _getNamePrivate() const {
			uintptr_t pThis = reinterpret_cast<uintptr_t>(this);
			if (pThis == 0) {
				std::string errMsg = "Attempted to access property '" "NamePrivate" "' on a null pointer of type '" "UObject" "'.";
				throw std::runtime_error(errMsg);
			}
			
			size_t offset = ((::size_t) & reinterpret_cast<char const volatile&>((((UObject*)0)->_NamePrivate)));
			uintptr_t pValue = pThis + offset;

			auto value = GDriver.Read<FName>(pValue);

			return value;
		};
		VERMILLION_PROP(UObject, UObject*, OuterPrivate, _OuterPrivate);

	private:
		uintptr_t* _VTable;
		uint32_t _ObjectFlags;
		uint32_t _ObjectIndex;
		class UClass* _ClassPrivate;
		FName _NamePrivate;
		UObject* _OuterPrivate;
	};
} // namespace Vermilion::UE
