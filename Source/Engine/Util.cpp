#include <Windows.h>
#include <cstdarg>
#include <cstdio>

namespace Util
{
	void DebugPrint(const char* format, ...)
	{
		constexpr int BUFFER_SIZE = 1024;
		static char buffer[BUFFER_SIZE];

		va_list args;
		va_start(args, format);

		int len = _vscprintf(format, args) + 1; // Include null-terminating char

		// Prevent buffer overflows
		if (len > BUFFER_SIZE) return;

		vsprintf_s(buffer, format, args);
		va_end(args);

		// Send to the debugger output window
		OutputDebugStringA(buffer);
	}
}