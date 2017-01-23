cbuffer constants : register(b0)
{
	float4x4 model_view_projection;
	uint id;
	uint is_highlighted;
	uint is_selected;
};

struct ps_output
{
	float4 color : SV_TARGET0;
	uint index : SV_TARGET1;
};

ps_output main(float4 position : SV_POSITION)
{
	ps_output result;
	result.color = float4(0.9f, 0.8f, 0.7f, 1.0f) + is_selected*float4(0.1f, 0.2f, 0.3f, 0.0f) - is_highlighted*float4(0.5f, 0.5f, 0.5f, 0.0f);
	result.index = id;
	return result;
}
