Texture2D diffuse_map : register(t0);
Texture2D normal_map : register(t1);
Texture2D tangent_map : register(t2);
Texture2D bump_normal_map : register(t3);
Texture2D shadow_map : register(t4);
Texture2D depth_buffer : register(t5);

SamplerState sampleClamp : register(s0);

#define NUM_LIGHTS 3
#define SHADOW_DEPTH_BIAS 0.00005f

struct LightState
{
	float3 position;
	float3 direction;
	float4 color;
	float4 falloff;
	float4x4 view_projection;
};

cbuffer g_buffer_constants : register(b0)
{
	float4x4 view_inverted;
	float4 perspective_values;
	float4 ambientColor;
	LightState lights[NUM_LIGHTS];
	uint sampleShadowMap;
	float screen_width;
	float screen_height;
};

float2 VSMain() : POSITION
{
	return float2(-1.0f, 1.0f);
}

struct PSInput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

[maxvertexcount(4)]
void GSMain(point float2 gin[1] : POSITION,
	inout TriangleStream<PSInput> triStream)
{
	float2 coordinates[4] =
	{
		float2(-1.0f, 1.0f),
		float2(1.0f, 1.0f),
		float2(-1.0f, -1.0f),
		float2(1.0f, -1.0f)
	};

	PSInput gout;
	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		gout.position = float4(coordinates[i], 0.0f, 1.0f);
		gout.uv = coordinates[i];
		triStream.Append(gout);
	}
}

float3 CalcPerPixelNormal(float3 bump_normal, float3 normal, float3 tangent)
{
	float3 vVertNormal = normalize(normal);
	float3 vVertTangent = normalize(tangent);
	float3 vVertBinormal = normalize(cross(vVertTangent, vVertNormal));
	float3x3 mTangentSpaceToWorldSpace = float3x3(vVertTangent, vVertBinormal, vVertNormal);

	bump_normal = 2.0f * bump_normal - 1.0f;
	return mul(bump_normal, mTangentSpaceToWorldSpace);
}

float4 CalcLightingColor(float3 vLightPos, float3 vLightDir, float4 vLightColor, float4 vFalloffs, float3 vPosWorld, float3 vPerPixelNormal)
{
	float3 vLightToPixelUnNormalized = vPosWorld - vLightPos;

	float fDist = length(vLightToPixelUnNormalized);
	float fDistFalloff = saturate((vFalloffs.x - fDist) / vFalloffs.y);
	float3 vLightToPixelNormalized = vLightToPixelUnNormalized / fDist;

	float fCosAngle = dot(vLightToPixelNormalized, vLightDir / length(vLightDir));
	float fAngleFalloff = saturate((fCosAngle - vFalloffs.z) / vFalloffs.w);
	float fNDotL = saturate(-dot(vLightToPixelNormalized, vPerPixelNormal));

	return vLightColor * fNDotL * fDistFalloff * fAngleFalloff;
}

float4 CalcUnshadowedAmountPCF2x2(int lightIndex, float4 world_space_position)
{
	float4 light_space_position = mul(world_space_position, lights[lightIndex].view_projection);
	light_space_position.xyz /= light_space_position.w;

	float2 vShadowTexCoord = 0.5f * light_space_position.xy + 0.5f;
	vShadowTexCoord.y = 1.0f - vShadowTexCoord.y;

	float vLightSpaceDepth = light_space_position.z - SHADOW_DEPTH_BIAS;

	float2 vShadowMapDims = float2(screen_width, screen_height);
	float4 vSubPixelCoords = float4(1.0f, 1.0f, 1.0f, 1.0f);
	vSubPixelCoords.xy = frac(vShadowMapDims * vShadowTexCoord);
	vSubPixelCoords.zw = 1.0f - vSubPixelCoords.xy;
	float4 vBilinearWeights = vSubPixelCoords.zxzx * vSubPixelCoords.wwyy;

	float2 vTexelUnits = 1.0f / vShadowMapDims;
	float4 vShadowDepths;
	vShadowDepths.x = shadow_map.Sample(sampleClamp, vShadowTexCoord).x;
	vShadowDepths.y = shadow_map.Sample(sampleClamp, vShadowTexCoord + float2(vTexelUnits.x, 0.0f)).x;
	vShadowDepths.z = shadow_map.Sample(sampleClamp, vShadowTexCoord + float2(0.0f, vTexelUnits.y)).x;
	vShadowDepths.w = shadow_map.Sample(sampleClamp, vShadowTexCoord + vTexelUnits).x;

	float4 vShadowTests = (vShadowDepths >= vLightSpaceDepth) ? 1.0f : 0.0f;
	return dot(vBilinearWeights, vShadowTests);
}

float4 calculate_world_position(float2 snorm_position, float depth)
{	
	float linear_depth = -perspective_values.z / (depth + perspective_values.w);

	float4 position;
	position.x = -snorm_position.x * perspective_values.x * linear_depth;
	position.y = -snorm_position.y * perspective_values.y * linear_depth;
	position.z = linear_depth;
	position.w = 1.0f;

	return mul(position, view_inverted);
}

float4 PSMain(PSInput input) : SV_TARGET0
{
	uint3 coord = uint3( ceil( input.position.x ), ceil( input.position.y ), 0);
	float4 diffuse_color = diffuse_map.Load( coord );
	float4 bump_normal = bump_normal_map.Load(coord);
	float4 normal = normal_map.Load(coord);
	float4 tangent = tangent_map.Load(coord);
	float depth = depth_buffer.Load(coord);
	float4 world_position = calculate_world_position(input.uv.xy, depth);

	float3 pixelNormal = CalcPerPixelNormal(bump_normal.xyz, normal.xyz, tangent.xyz);
	float4 totalLight = ambientColor;

	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		float4 lightPass = CalcLightingColor(lights[i].position, lights[i].direction, lights[i].color, lights[i].falloff, world_position.xyz, pixelNormal);
		if (sampleShadowMap && i == 0)
		{
			lightPass *= CalcUnshadowedAmountPCF2x2(i, world_position);
		}
		totalLight += lightPass;
	}

	return diffuse_color * saturate(totalLight);
}