#pragma once

namespace Vermilion::UE {
	template <typename T>
    class TArray {
    public:
        TArray() : Data(nullptr), Count(0), Max(0) {}
        TArray(T* data, int32_t count, int32_t max) : Data(data), Count(count), Max(max) {}
        TArray(T* data, int32_t count) : Data(data), Count(count), Max(count) {}
        TArray(T* data, size_t count) : Data(data), Count(count), Max(count) {}
        TArray(const TArray& other) : Data(other.Data), Count(other.Count), Max(other.Max) {}

        T operator[](int32_t index) const {
            if (index < 0 || index >= Count) {
                std::string msg = std::format(
                    "TArray index out of range: index {}, count {}",
                    index,
                    Count
                );

                throw std::out_of_range(msg);
            }

            return GDriver.Read<T>(Data + index);
        }

        struct Iterator {
            T* Current;
            int32_t Index;

            Iterator(T* ptr, int32_t index) : Current(ptr), Index(index) {}

            T operator*() {
                return GDriver.Read<T>(reinterpret_cast<uintptr_t>(Current));
            }

            Iterator& operator++() {
                ++Current;
                ++Index;
                return *this;
            }

            bool operator!=(const Iterator& other) const {
                return Current != other.Current;
            }
        };

        Iterator begin() { return Iterator(Data, 0); }
        Iterator end() { return Iterator(Data + Count, Count); }

        std::string ToString() const {
            return std::format("TArray(Count: {}, Max: {})", Count, Max);
        }

        T* Data;
        int32_t Count;
        int32_t Max;
    };
}
