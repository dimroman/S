#include "common.hlsl"


struct ps_output
{
	float4 color : SV_TARGET0;
	uint index : SV_TARGET1;
};

ps_output main(float4 position : SV_POSITION, uint instance_id : SV_InstanceID)
{
	ps_output result;
	result.color = colors[id+instance_id];
	result.index = id+instance_id;
	return result;
}
