cbuffer object_id : register(b0)
{
	uint id;
};

cbuffer model_transform_constants : register(b1)
{
	float4x4 model_transforms[RENDER_OBJECT_INSTANCES_COUNT];
}

cbuffer color_constants : register(b2)
{
	float4 colors[RENDER_OBJECT_INSTANCES_COUNT];
	float4x4 view_projection_transform;
};