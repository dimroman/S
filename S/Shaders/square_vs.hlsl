cbuffer constants : register(b0)
{
	float4x4 model_view_projection;
	uint id;
	uint is_highlighted;
	uint is_selected;
};

float4 main(float2 position : POSITION) : SV_POSITION
{
	return mul(float4(position, 0.0f, 1.0f), model_view_projection);
}
