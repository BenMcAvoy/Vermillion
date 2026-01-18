#pragma once

#define VERMILION_PROP(ClassType, Type, PropName, MemberName) \
    __declspec(property(get = _get##PropName)) Type PropName; \
    Type _get##PropName() const { \
        do { \
        uintptr_t pThis = reinterpret_cast<uintptr_t>(this); \
        if (pThis == 0) { \
            std::string errMsg = "Attempted to access property '" #PropName "' on a null pointer of type '" #ClassType "'."; \
			throw std::runtime_error(errMsg); \
        } \
        size_t offset = offsetof(ClassType, MemberName); \
        uintptr_t pValue = pThis + offset; \
        auto value = GDriver.Read<Type>(pValue); \
        return value; \
        } while (0); \
    }

#define VERMILION_PROP_OFFSET(ClassType, Type, PropName, OffsetVal) \
    __declspec(property(get = _get##PropName)) Type PropName; \
    Type _get##PropName() const { \
        uintptr_t pThis = reinterpret_cast<uintptr_t>(this); \
        if (pThis == 0) { \
            std::string errMsg = "Attempted to access property '" #PropName "' on a null pointer of type '" #ClassType "'."; \
            throw std::runtime_error(errMsg); \
        } \
        size_t offset = (OffsetVal); \
        uintptr_t pValue = pThis + offset; \
        auto value = GDriver.Read<Type>(pValue); \
        return value; \
    }

#define VERMILION_ASSERT_SIZE(ClassType, Size) static_assert(sizeof(ClassType) == Size);
