#include "common.hlsl"


struct ps_output
{
	float4 color : SV_TARGET0;
	uint index : SV_TARGET1;
};

ps_output main(float4 position : SV_POSITION)
{
	ps_output result;
	result.color = colors[id];
	result.index = id;
	return result;
}
