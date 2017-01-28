cbuffer object_id : register(b0)
{
	uint id;
};

struct per_object_constants
{
	float4 color;
	float2 position;
	float width;
	float height;
};

cbuffer per_frame_constants : register(b1)
{
	float4x4 model_view_projection;
	per_object_constants object_constants[RENDER_OBJECTS_COUNT];
};