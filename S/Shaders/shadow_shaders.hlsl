cbuffer shadows_constant_buffer : register(b0)
{
	float4x4 model_view_projection;
};

float4 VSMain(float3 position : POSITION) : SV_POSITION
{
	float4 newPosition = float4(position, 1.0f);
	newPosition = mul(newPosition, model_view_projection);	
	return newPosition;
}
