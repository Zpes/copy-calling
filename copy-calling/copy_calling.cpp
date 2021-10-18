#include "copy_calling.h"

void* copy_calling::create::create_documented_copy(LPCSTR module, LPCSTR function)
{
    HMODULE h_module = GetModuleHandleA(module);
    std::vector<BYTE> v_bytes;

    if (h_module != INVALID_HANDLE_VALUE)
    {
        void* address = GetProcAddress(h_module, function);

        if (!address)
            return nullptr;

        for (int i = 0; true; i++)
        {
            void* b_address = (void*)((uintptr_t)address + (uintptr_t)(i * sizeof(BYTE)));

            v_bytes.push_back(*(BYTE*)b_address);

            if (*(BYTE*)b_address == 0xC3)
                break;
        }

		void* alloc = copy_calling::utility::allocate_memory_close_to_address(address, v_bytes.size());

        if (alloc)
        {
            for (int i = 0; i < v_bytes.size(); i++)
            {
                memcpy((void*)((uintptr_t)alloc + (uintptr_t)(i * sizeof(BYTE))), (void*)&v_bytes[i], sizeof(BYTE));
            }

            return alloc;
        }
    }

    return nullptr;
}

void* copy_calling::create::create_undocumented_copy(void* address)
{
    if (!address)
        return nullptr;

    std::vector<BYTE> v_bytes;

    for (int i = 0; true; i++)
    {
        void* b_address = (void*)((uintptr_t)address + (uintptr_t)(i * sizeof(BYTE)));

        v_bytes.push_back(*(BYTE*)b_address);

        if (*(BYTE*)b_address == 0xC3)
            break;
    }

    void* alloc = copy_calling::utility::allocate_memory_close_to_address(address, v_bytes.size());

    if (alloc)
    {
        for (int i = 0; i < v_bytes.size(); i++)
        {
            memcpy((void*)((uintptr_t)alloc + (uintptr_t)(i * sizeof(BYTE))), (void*)&v_bytes[i], sizeof(BYTE));
        }

        return alloc;
    }

    return nullptr;
}

void* copy_calling::utility::allocate_memory_close_to_address(void* address, size_t size)
{
	SYSTEM_INFO system_info; GetSystemInfo(&system_info);
	const uintptr_t page_size = system_info.dwPageSize;

	uintptr_t start_adress = (uintptr_t(address) & ~(page_size - 1));
	uintptr_t min = min(start_adress - 0x7FFFFF00, (uintptr_t)system_info.lpMinimumApplicationAddress);
	uintptr_t max = max(start_adress + 0x7FFFFF00, (uintptr_t)system_info.lpMaximumApplicationAddress);

	uintptr_t start_page = (start_adress - (start_adress % page_size));

	uintptr_t page = 1;
	while (true)
	{
		uintptr_t byte_offset = page * page_size;
		uintptr_t high = start_page + byte_offset;
		uintptr_t low = (start_page > byte_offset) ? start_page - byte_offset : 0;

		bool stop_point = high > max && low < min;

		if (!low)
			continue;

		if (high < max)
		{
			void* outAddr = VirtualAlloc((void*)high, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			if (outAddr)
				return outAddr;
		}

		if (low > min)
		{
			void* outAddr = VirtualAlloc((void*)low, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			if (outAddr)
				return outAddr;
		}

		page += 1;

		if (stop_point)
			break;
	}

	return nullptr;
}
