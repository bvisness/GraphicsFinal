#version 430
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable
#extension GL_ARB_shader_atomic_counters: enable

#define NUM_VERTICES 36
#define NUM_WORK_GROUPS 1
#define WORK_GROUP_SIZE (NUM_VERTICES / NUM_WORK_GROUPS)
#define M_PI 3.141592653

layout(std140) coherent buffer SSBO
{
	vec4 vPositions[];
};
layout(binding = 0, offset = 0) uniform atomic_uint uEveryOther;
layout(local_size_x = NUM_VERTICES, local_size_y = 1, local_size_z = 1) in;

void main() {
	uint i = gl_GlobalInvocationID.x;
	uint t_i = (i + 1) / 2;
	float t = t_i;
	t = (2 * t / NUM_VERTICES);

	uint everyOther = atomicCounter(uEveryOther);
	float theta = 2 * M_PI * t;
	float z;
	if (everyOther == 0) {
		z = atomicCounterIncrement(uEveryOther);
	} else {
		z = atomicCounterDecrement(uEveryOther);
	}
	
	//float counter = atomicCounter(uEveryOther);
	//z = everyOther;
	vPositions[i] = vec4(cos(theta), sin(theta), z / 10.0, 1);
}

// Ask about memory barriers
