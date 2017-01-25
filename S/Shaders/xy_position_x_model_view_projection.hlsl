cbuffer per_frame_constants : register(b0)
{
	float4x4 model_view_projection;
};

cbuffer static_constants : register(b2)
{
	float2 object_position;
	float object_width;
	float object_height;
	uint id;
}

float4 main(float2 position : POSITION) : SV_POSITION
{
	float2 world_position = float2(position.x*object_width, position.y*object_height) + object_position;
	return mul(float4(world_position, 0.0f, 1.0f), model_view_projection);
}
