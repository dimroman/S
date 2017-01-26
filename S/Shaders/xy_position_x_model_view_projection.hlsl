#include "common.hlsl"

float4 main(float2 position : POSITION) : SV_POSITION
{
	float2 world_position = float2(position.x*object_width, position.y*object_height) + object_position;
	return mul(float4(world_position, 0.0f, 1.0f), model_view_projection);
}
