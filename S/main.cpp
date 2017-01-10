#include "application.h"

int WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	application main_application(hInstance);
	return main_application.run();
}