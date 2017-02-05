#include "common.hlsl"

float4 main(float2 position : POSITION) : SV_POSITION
{
	float4x4 model_view_projection = mul(model_transforms[id], view_projection_transform);
	return mul(float4(position, 0.0f, 1.0f), model_view_projection);
}
