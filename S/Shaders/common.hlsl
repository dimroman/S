cbuffer object_id : register(b0)
{
	uint id;
};

cbuffer model_view_projection_constants : register(b1)
{
	float4x4 model_view_projections[RENDER_OBJECT_INSTANCES_COUNT];
}

cbuffer color_constants : register(b2)
{
	float4 colors[RENDER_OBJECT_INSTANCES_COUNT];
};