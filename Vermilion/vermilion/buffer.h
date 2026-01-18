#pragma once

namespace Vermilion {
	template <typename T>
	class DoubleBuffer {
	
	private:
		T bufs[2];
		std::atomic<size_t> activeIndex{ 0 };
	};
} // namespace Vermilion
