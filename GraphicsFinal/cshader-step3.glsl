#version 430
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable

#define CHUNK_SIZE 64

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
layout(std430, binding = 33) coherent buffer ssboDepthCounts
{
	int depthCounts[];
};
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

int arrayIndex(int depth, uint i) {
	return (depth * depths.length()) + int(i);
}

void main() {
	int stringStart = int(startEndIndices[gl_WorkGroupID.x].x);
	int stringEnd = int(startEndIndices[gl_WorkGroupID.x].y);
	int startDepth = depths[gl_WorkGroupID.x];

	int depth = startDepth;
	for (int i = stringStart; i <= stringEnd; i++) {
		if (chars[i] == OP_PUSH) {
			depthCounts[arrayIndex(depth, gl_WorkGroupID.x)] += 1;
			depth++;
		} else if (chars[i] == OP_POP) {
			depth--;
			depthCounts[arrayIndex(depth, gl_WorkGroupID.x)] += 1;
		}
	}
}
