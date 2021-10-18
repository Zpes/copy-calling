#include "copy_calling.h"

BOOL(__stdcall* c_IsDebuggerpresent)() = nullptr;

BOOLEAN WINAPI main()
{
	*(void**)&c_IsDebuggerpresent = copy_calling::create::create_documented_copy("kernelbase.dll", "IsDebuggerPresent");

	while (1)
	{
		printf_s("debugger -> %d\n", c_IsDebuggerpresent());

		Sleep(2000);
	}
}