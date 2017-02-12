#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#include "options.h"
#include "graphics.h"
#include "logic.h"
#include "camera.h"
#include "input.h"
#include "storage.h"
#include "math.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class application
{
public:
	application();
	void run();
private:
	storage				m_storage;
	options				m_options;
	graphics			m_graphics;
	camera				m_camera;
	input				m_input;
	logic				m_logic;
};


#endif // #ifndef APPLICATION_H_INCLUDED