Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);

SamplerState sampleWrap : register(s0);

cbuffer transform_constants_buffer : register(b0)
{
	float4x4 model;
	float4x4 view_projection;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float4 worldpos : POSITION;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

PSInput VSMain(float3 position : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD0, float3 tangent : TANGENT)
{
	PSInput result;

	float4 newPosition = float4(position, 1.0f);

	normal.z *= -1.0f;
	newPosition = mul(newPosition, model);

	result.worldpos = newPosition;

	newPosition = mul(newPosition, view_projection);

	result.position = newPosition;
	result.uv = uv;
	result.normal = normal;
	result.tangent = tangent;

	return result;
}

struct PSOutput
{
	float4 diffuse_color : SV_TARGET1;
	float4 normal : SV_TARGET2;
	float4 tangent : SV_TARGET3;
	float4 bump_normal : SV_TARGET4;
};

PSOutput PSMain(PSInput input)
{
	PSOutput result;

	result.diffuse_color = diffuseMap.Sample(sampleWrap, input.uv);
	result.normal = float4(input.normal, 1.0f);
	result.tangent = float4(input.tangent, 1.0f);
	result.bump_normal = float4((float3)normalMap.Sample(sampleWrap, input.uv), 1.0f);

	return result;
}
