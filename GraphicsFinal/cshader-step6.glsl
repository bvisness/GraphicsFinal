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
layout(std430, binding = 35) coherent buffer ssboBucketSortArray
{
	int bucketSortArray[];
};
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

void main() {
	int bucketArrayStart = int(startEndIndices[gl_WorkGroupID.x].x);
	int bucketArrayEnd = int(startEndIndices[gl_WorkGroupID.x].y);

	for (int i = bucketArrayStart; i <= bucketArrayEnd; i++) {
		if (i % 2 == 0) {
			int pushLocation = bucketSortArray[i];
			chars[pushLocation] = -bucketSortArray[i + 1];
		}
	}
}
