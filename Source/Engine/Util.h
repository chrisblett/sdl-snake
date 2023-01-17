#pragma once

#define DBG_PRINT_SEPARATOR(name) \
do { \
	\
		Util::DebugPrint("***** [%s] *****\n", (name)); \
} while(0) \

namespace Util
{
	// Send a formatted string the debugger output window
	void DebugPrint(const char* format, ...);
}
