#version 430
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable

#define NUM_VERTICES 360
#define M_PI 3.141592653

layout(std140) buffer SSBO
{
	vec4 vPositions[ ]; // array of structures
};
layout(local_size_x = 6, local_size_y = 1, local_size_z = 1) in;

void main() {
	uint gid = gl_GlobalInvocationID.x;

	float gid_f = gid;
	float t = (gid_f / NUM_VERTICES);
	t = t * 2 * M_PI;
	vPositions[gid] = vec4(cos(t), sin(t), 0, 1);
	//vPositions[gid].xy = vec2(gid_f / NUM_VERTICES, max(0.5, gid_f / NUM_VERTICES));
}
