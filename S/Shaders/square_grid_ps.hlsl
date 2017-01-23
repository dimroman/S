cbuffer constants : register(b0)
{
	float4x4 model_view_projection;
	uint id;
};

struct ps_input
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

struct ps_output
{
	float4 color : SV_TARGET0;
	uint index : SV_TARGET1;
};

ps_output main(ps_input input)
{
	ps_output result;
	result.color = input.color;
	result.index = id;
	return result;
}
