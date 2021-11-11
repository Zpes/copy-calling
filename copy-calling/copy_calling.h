#pragma once
#include <windows.h>
#include <vector>

#pragma warning (disable : 6387)

namespace copy_calling
{
	namespace create
	{
		void* create_documented_copy(LPCSTR module, LPCSTR function);
		void* create_undocumented_copy(void* address);
	}

	namespace utility
	{
		void* allocate_memory_close_to_address(void* address, std::size_t size);
	}
}