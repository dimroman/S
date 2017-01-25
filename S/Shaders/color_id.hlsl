cbuffer per_object_per_frame_constants : register(b1)
{
	float4 color;
};

cbuffer static_constants : register(b2)
{
	float2 object_position;
	float object_width;
	float object_height;
	uint id;
}

struct ps_output
{
	float4 color : SV_TARGET0;
	uint index : SV_TARGET1;
};

ps_output main(float4 position : SV_POSITION)
{
	ps_output result;
	result.color = color;
	result.index = id;
	return result;
}
