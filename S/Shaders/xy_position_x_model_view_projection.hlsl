#include "common.hlsl"

float4 main(float2 position : POSITION) : SV_POSITION
{
	float2 world_position = float2(position.x*object_constants[id].width, position.y*object_constants[id].height) + object_constants[id].position;
	return mul(float4(world_position, 0.0f, 1.0f), model_view_projection);
}
