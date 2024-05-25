#ifndef EIC_MEMMANG_H
#define EIC_MEMMANG_H

#undef NDEBUG
#include <cassert>
#include <iostream>

namespace eic
{
	size_t total_memory_blocks = 0, free_memory_blocks = 0;

	template <typename T>
	inline T* ealloc(size_t amount)
	{
		total_memory_blocks++;
		void* temp = malloc(sizeof(T) * amount);
		assert(temp);
		return reinterpret_cast<T*>(temp);
	}

	inline void efree(void* block)
	{
		free_memory_blocks++;
		free(block);
	}

	inline std::ostream& memory_state(std::ostream& os)
	{
		return os << "Memory State (Free/Total): " << free_memory_blocks << '/' << total_memory_blocks;
	}
}

#endif // !EIC_MEMMANG_H
