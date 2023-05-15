#include "ConsoleDebug.h"

ConsoleDebug* ConsoleDebug::console = NULL;
bool ConsoleDebug::consoleActive = true;

void ConsoleDebug::Init()
{

}

void debug(LPCTSTR str)
{
	if (ConsoleDebug::consoleActive)
	{
		if (ConsoleDebug::console == NULL)
			ConsoleDebug::console = new ConsoleDebug(true, CP_ACP); // 디버그용
		*ConsoleDebug::console << str;
	}
}