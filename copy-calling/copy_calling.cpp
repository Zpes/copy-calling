#include "copy_calling.h"

#include <array>
#include <map>

// [byte, size (not including opcode)]
static const std::map<std::uint8_t, std::uint8_t> return_bytes{
	{ 0xC3, 0 },
	{ 0xC2, 2 },
};

// can use std::to_array in C++20, otherwise declare a user-defined literal and specify no template params.
static const std::array<std::uint8_t, 2> alignment_bytes{
	0xCC,
	0x90
};

void* copy_calling::create::create_documented_copy(const LPCSTR module, const LPCSTR function)
{
	const auto handle = GetModuleHandleA(module);

	// not doing handle standalone here may throw a warning
	return handle && handle != INVALID_HANDLE_VALUE ? create_undocumented_copy(GetProcAddress(handle, function)) : nullptr;
}

void* copy_calling::create::create_undocumented_copy(void* address)
{
	if (!address)
		return nullptr;

	std::vector<BYTE> v_bytes;

	for (std::uintptr_t i = 0u;; i++)
	{
		const auto b_address = reinterpret_cast<BYTE*>(reinterpret_cast<uintptr_t>(address) + (i * sizeof(BYTE)));
		auto kill = false;

		v_bytes.push_back(*b_address);

		// in C++17 you can use structured bindings here, since the project is C++14 I'll stick to referencing the pair elements
		for (const auto& elem : return_bytes)
		{
			const auto& opcode = elem.first;
			const auto& size = elem.second;


			if (*(b_address - size) == opcode)
			{
				// checks if at total end of function
				// not going to reference 1 byte
				for (const auto alignment : alignment_bytes)
					if (*(b_address + 1) == alignment)
					{
						kill = true;
						break;
					}

				// exit parent loop
				if (kill)
					break;
			}
		}

		if (kill)
			break;
	}

	const auto alloc = copy_calling::utility::allocate_memory_close_to_address(address, v_bytes.size());

	if (alloc)
	{
		std::memcpy(alloc, v_bytes.data(), v_bytes.size());

		return alloc;
	}

	return nullptr;
}

void* copy_calling::utility::allocate_memory_close_to_address(void* address, const std::size_t size)
{
	SYSTEM_INFO system_info{};
	GetSystemInfo(&system_info);

	const auto page_size = static_cast<std::uintptr_t>(system_info.dwPageSize);

	const auto start_adress = (reinterpret_cast<std::uintptr_t>(address) & ~(page_size - 1));
	const auto min = min(start_adress - 0x7FFFFF00, reinterpret_cast<std::uintptr_t>(system_info.lpMinimumApplicationAddress));
	const auto max = max(start_adress + 0x7FFFFF00, reinterpret_cast<std::uintptr_t>(system_info.lpMaximumApplicationAddress));

	const auto start_page = (start_adress - (start_adress % page_size));

	std::size_t page = 1;
	while (true)
	{
		const auto byte_offset = page * page_size;
		const auto high = start_page + byte_offset;
		const auto low = (start_page > byte_offset) ? start_page - byte_offset : 0;

		const auto stop_point = high > max && low < min;

		if (!low)
			continue;

		if (high < max)
		{
			auto const out_addr = VirtualAlloc(reinterpret_cast<void*>(high), size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			if (out_addr)
				return out_addr;
		}

		if (low > min)
		{
			auto const out_addr = VirtualAlloc(reinterpret_cast<void*>(low), size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			if (out_addr)
				return out_addr;
		}

		page++;

		if (stop_point)
			break;
	}

	return nullptr;
}
