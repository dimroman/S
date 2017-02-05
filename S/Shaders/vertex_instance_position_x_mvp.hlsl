#include "common.hlsl"

struct vertex_shader_input
{
	float2 vertex_position : POSITION;
	uint instance_id : SV_InstanceID;
};

struct vertex_shader_output
{
	float4 position : SV_POSITION;
	uint instance_id : SV_InstanceID;
};

vertex_shader_output main(vertex_shader_input input)
{
	vertex_shader_output result;
	float4x4 model_view_projection = mul(model_transforms[id+input.instance_id], view_projection_transform);
	result.position = mul(float4(input.vertex_position, 0.0f, 1.0f), model_view_projection);
	result.instance_id = input.instance_id;
	return result;
}
