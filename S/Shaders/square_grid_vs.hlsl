cbuffer constants : register(b0)
{
	float4x4 model_view_projection;
	uint id;
};

struct vs_output
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

vs_output main(float2 position : POSITION, float3 color : COLOR)
{
	vs_output result;
	result.position = mul(float4(position, 0.0f, 1.0f), model_view_projection);
	result.color = float4(color, 1.0f);
	return result;
}
