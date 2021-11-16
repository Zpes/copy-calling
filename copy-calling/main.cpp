#include "copy_calling.h"

int main()
{
	BOOL(__stdcall * is_debugger_present)() = nullptr;

	*reinterpret_cast<void**>(&is_debugger_present) = copy_calling::create::create_documented_copy("kernelbase.dll", "IsDebuggerPresent");

	while (true)
	{
		std::printf("debugger -> %d\n", is_debugger_present());

		Sleep(2000);
	}
}