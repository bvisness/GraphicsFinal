#version 430
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable

#define OP_PUSH 91
#define OP_POP 93

#define POSITION 0
#define HEADING 1
#define LEFT 2
#define UP 3

#define DEG_TO_RAD 0.0174533

layout(std430, binding = 30) coherent buffer ssboInputString
{
	int chars[];
};
layout(std430, binding = 37) coherent buffer ssboDelta
{
	float delta;
};
layout(std430, binding = 38) coherent buffer ssboWorkqueueMatrices
{
	mat4 startMatrices[];
};
layout(std430, binding = 39) coherent buffer ssboWorkqueueStringPositions
{
	int stringPositions[];
};
layout(std430, binding = 40) coherent buffer ssboVertexPositions
{
    vec4 vPositions[];
};
layout(binding = 3) uniform atomic_uint acWorkqueueIndex;
layout(binding = 4) uniform atomic_uint acVertexPairIndex;
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

mat3 GetAxes(mat4 turtle) {
	mat3 result;
	result[0] = turtle[HEADING].xyz;
	result[1] = turtle[LEFT].xyz;
	result[2] = turtle[UP].xyz;
	return result;
}

mat4 SetAxes(mat4 turtle, mat3 axes) {
	turtle[HEADING] = vec4(axes[0], 0);
	turtle[LEFT] = vec4(axes[1], 0);
	turtle[UP] = vec4(axes[2], 0);
	return turtle;
}

mat3 RotateU(float alpha) {
	float alphaR = alpha * DEG_TO_RAD;
	mat3 result;
	result[0] = vec3(cos(alphaR), sin(alphaR), 0);
	result[1] = vec3(-sin(alphaR), cos(alphaR), 0);
	result[2] = vec3(0, 0, 1);
	return transpose(result); // I copied from the C++, so to save time I will just transpose
}

mat4 RotateU(mat4 turtle, float alpha) {
	mat3 newAxes = GetAxes(turtle) * RotateU(alpha);
	return SetAxes(turtle, newAxes);
}

mat3 RotateL(float alpha) {
	float alphaR = alpha * DEG_TO_RAD;
	mat3 result;
	result[0] = vec3(cos(alphaR), 0, -sin(alphaR));
	result[1] = vec3(0, 1, 0);
	result[2] = vec3(sin(alphaR), 0, cos(alphaR));
	return transpose(result);
}

mat4 RotateL(mat4 turtle, float alpha) {
	mat3 newAxes = GetAxes(turtle) * RotateL(alpha);
	return SetAxes(turtle, newAxes);
}

mat3 RotateH(float alpha) {
	float alphaR = alpha * DEG_TO_RAD;
	mat3 result;
	result[0] = vec3(1, 0, 0);
	result[1] = vec3(0, cos(alphaR), -sin(alphaR));
	result[2] = vec3(0, sin(alphaR), cos(alphaR));
	return transpose(result);
}

mat4 RotateH(mat4 turtle, float alpha) {
	mat3 newAxes = GetAxes(turtle) * RotateH(alpha);
	return SetAxes(turtle, newAxes);
}

// Finally, the main method!
void main() {
	mat4 turtle = startMatrices[gl_WorkGroupID.x];
	int startPosition = stringPositions[gl_WorkGroupID.x];

	for (int i = startPosition; i < chars.length(); i++) {
		if (chars[i] < 0) {
			// Actually a push! Negate it and we will have the location
			// of the corresponding pop.
			mat4 newStartMatrix = turtle;
			int newStringPosition = i + 1;

			uint workqueueAddId = atomicCounterIncrement(acWorkqueueIndex);
			startMatrices[workqueueAddId] = newStartMatrix;
			stringPositions[workqueueAddId] = newStringPosition;

			int popIndex = -chars[i];
			i = popIndex; // This will be incremented when we go through the loop

			continue;
		}

		if (chars[i] == 70) { // F
			uint vertexPairIndex = atomicCounterIncrement(acVertexPairIndex);
			vPositions[vertexPairIndex * 2] = turtle[POSITION];

			turtle[POSITION] += turtle[HEADING];

			vPositions[vertexPairIndex * 2 + 1] = turtle[POSITION];
		}
		else if (chars[i] == 102) { // f
			turtle[POSITION] += turtle[HEADING];
		}
		else if (chars[i] == 43) { // +
			turtle = RotateU(turtle, delta);
		}
		else if (chars[i] == 45) { // -
			turtle = RotateU(turtle, -delta);
		}
		else if (chars[i] == 38) { // &
			turtle = RotateL(turtle, delta);
		}
		else if (chars[i] == 94) { // ^
			turtle = RotateL(turtle, -delta);
		}
		else if (chars[i] == 92) { // \
			turtle = RotateH(turtle, delta);
		}
		else if (chars[i] == 47) { // /
			turtle = RotateH(turtle, -delta);
		}
		else if (chars[i] == 124) { // |
			turtle = RotateU(turtle, 180);
		}
		else if (chars[i] == 93) { // ]
			break;
		}
	}
}
