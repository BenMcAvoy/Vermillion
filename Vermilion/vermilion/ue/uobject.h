#pragma once

namespace Vermilion::UE {
	class UObject {
	public:
		VERMILION_PROP(UObject, uintptr_t*, VTable, _VTable);
		VERMILION_PROP(UObject, uint32_t, ObjectFlags, _ObjectFlags);
		VERMILION_PROP(UObject, uint32_t, ObjectIndex, _ObjectIndex);
		VERMILION_PROP(UObject, class UClass*, ClassPrivate, _ClassPrivate);
		VERMILION_PROP(UObject, FName, NamePrivate, _NamePrivate);
		VERMILION_PROP(UObject, UObject*, OuterPrivate, _OuterPrivate);

	private:
		uintptr_t* _VTable;
		uint32_t _ObjectFlags;
		uint32_t _ObjectIndex;
		class UClass* _ClassPrivate;
		FName _NamePrivate;
		UObject* _OuterPrivate;
	};

	VERMILION_ASSERT_SIZE(UObject, 0x28);
} // namespace Vermilion::UE
