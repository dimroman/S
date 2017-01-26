cbuffer global_per_frame_constants : register(b0)
{
	float4x4 model_view_projection;
};

cbuffer per_object_per_frame_constants : register(b1)
{
	float4 color[RENDER_OBJECTS_COUNT];
};

cbuffer static_constants : register(b2)
{
	float2 object_position;
	float object_width;
	float object_height;
};

cbuffer object_id : register(b3)
{
	uint id;
};