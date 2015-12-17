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
layout(std430, binding = 33) coherent buffer ssboDepthCounts
{
	int depthCounts[];
};
layout(std430, binding = 34) coherent buffer ssboBucketOffsets
{
	int bucketOffsets[];
};
layout(std430, binding = 35) coherent buffer ssboBucketSortArray
{
	int bucketSortArray[];
};
layout(std430, binding = 36) coherent buffer ssboBucketStartIndices
{
	int bucketStartIndices[];
};
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

int opCounts[64]; // 64 is arbitrary. I sincerely hope I never have an L-system that goes 65 levels deep.

int arrayIndex(int depth, uint i) {
	return (depth * depths.length()) + int(i);
}

void registerOp(int opIndex, int depth) {
	int bucketStartIndex = bucketStartIndices[depth];
	int bucketOffset = bucketOffsets[arrayIndex(depth, gl_WorkGroupID.x)];
	int opOffset = opCounts[depth];
	bucketSortArray[bucketStartIndex + bucketOffset + opOffset] = opIndex;

	opCounts[depth]++;
}

void main() {
	int stringStart = int(startEndIndices[gl_WorkGroupID.x].x);
	int stringEnd = int(startEndIndices[gl_WorkGroupID.x].y);
	int startDepth = depths[gl_WorkGroupID.x];
	
	for (int i = 0; i < opCounts.length(); i++) {
		opCounts[i] = 0;
	}

	int depth = startDepth;
	for (int i = stringStart; i <= stringEnd; i++) {
		if (chars[i] == OP_PUSH) {
			registerOp(i, depth);
			depth++;
		} else if (chars[i] == OP_POP) {
			depth--;
			registerOp(i, depth);
		}
	}
}
