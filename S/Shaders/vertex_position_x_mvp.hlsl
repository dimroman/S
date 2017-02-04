#include "common.hlsl"

float4 main(float2 position : POSITION) : SV_POSITION
{
	return mul(float4(position, 0.0f, 1.0f), model_view_projections[id]);
}
