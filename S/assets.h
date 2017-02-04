#ifndef ASSETS_H_INCLUDED
#define ASSETS_H_INCLUDED

#include "math.h"

namespace assets
{

static math::float2 rectangle_vertices[]{
	{ 0.5f, 0.5f },
	{ -0.5f, 0.5f },
	{ 0.5f, -0.5f },
	{ -0.5f, -0.5f }
};

static unsigned rectangle_indices[]{
	0, 1, 2,
	2, 1, 3
};

static math::float2 hexagon_vertices[]{
	{ 0.0f, 0.0f },
	{ 0.0f, 1.0f },
	{ 0.866025f, 0.5f },
	{ 0.866025f, -0.5f },
	{ 0.0f, -1.0f },
	{ -0.866025f, -0.5f },
	{ -0.866025f, 0.5f }
};

static unsigned hexagon_indices[]{
	0, 2, 1,
	0, 3, 2,
	0, 4, 3,
	0, 5, 4,
	0, 6, 5,
	0, 1, 6
};

static math::float2 hexagon_frame_vertices[]{
	{ 0.0f, 1.0f },
	{ 0.866025f, 0.5f },
	{ 0.866025f, -0.5f },
	{ 0.0f, -1.0f },
	{ -0.866025f, -0.5f },
	{ -0.866025f, 0.5f }
};

static unsigned hexagon_frame_indices[]{
	0, 1,
	1, 2,
	2, 3,
	3, 4,
	4, 5,
	5, 0
};

static D3D12_INPUT_ELEMENT_DESC const vertex_position_float2[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
};

static D3D12_INPUT_LAYOUT_DESC const vertex_position_float2_layout{ vertex_position_float2, _countof(vertex_position_float2) };



}

#endif // #ifndef ASSETS_H_INCLUDED