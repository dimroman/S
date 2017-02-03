cbuffer object_id : register(b0)
{
	uint id;
};

cbuffer static_constants : register(b1)
{
	float4x4 model_view_projections[RENDER_OBJECTS_COUNT];
}

cbuffer per_frame_constants : register(b2)
{
	float4 colors[RENDER_OBJECTS_COUNT];
};