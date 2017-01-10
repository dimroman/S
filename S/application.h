#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#include "options.h"
#include "graphics.h"

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class application
{
public:
	application(HINSTANCE handle_instance);
	application(const application& rhs) = delete;
	application& operator=(const application& rhs) = delete;
	~application();

	int run();

private:
	HINSTANCE m_application_instance_handle = nullptr;
	HWND      m_main_window_handle = nullptr;

	graphics			m_graphics;
};


#endif // #ifndef APPLICATION_H_INCLUDED