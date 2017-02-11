#include "application.h"

int WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	application main_application;
	main_application.run();
	return 0;
}