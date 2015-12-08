#version 430 compatibility
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable

#define NUM_VERTICES 1000
#define M_PI 3.141592653

layout(std140, binding = 4) buffer Pos
{
	vec4 vPositions[ ]; // array of structures
};
layout(local_size_x = 6, local_size_y = 1, local_size_z = 1) in;

void main() {
	uint gid = gl_GlobalInvocationID.x;

	float t = (gid / NUM_VERTICES);
	t = t * 2 * M_PI;
	vPositions[gid].x = cos(t);
	vPositions[gid].y = sin(t);
}