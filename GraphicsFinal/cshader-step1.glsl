#version 430
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable

#define OP_PUSH 91
#define OP_POP 93

layout(std430, binding = 30) coherent buffer ssboInputString
{
	int chars[];
};
layout(std430, binding = 31) coherent buffer ssboInputStringStartsEnds
{
	vec2 startEndIndices[];
};
layout(std430, binding = 32) coherent buffer ssboChunkDepths
{
	int depths[];
};
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

void main() {
	int stringStart = int(startEndIndices[gl_WorkGroupID.x].x);
	int stringEnd = int(startEndIndices[gl_WorkGroupID.x].y);

	for (int i = stringStart; i <= stringEnd; i++) {
		if (chars[i] == OP_PUSH) {
			depths[gl_WorkGroupID.x] += 1;
		} else if (chars[i] == OP_POP) {
			depths[gl_WorkGroupID.x] -= 1;
		}
	}
}
