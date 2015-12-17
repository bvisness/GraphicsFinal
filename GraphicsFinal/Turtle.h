
#ifndef GraphicsFinal_Turtle_h
#define GraphicsFinal_Turtle_h

#include <GL/Angel.h>

#define POSITION 0
#define HEADING 1
#define LEFT 2
#define UP 3

mat4 NewTurtle() {
	mat4 turtle = mat4();
	turtle[POSITION] = vec4(0, 0, 0, 1); // Position
	turtle[HEADING] = vec4(0, 1, 0, 0); // Heading
	turtle[LEFT] = vec4(-1, 0, 0, 0); // Left
	turtle[UP] = vec4(0, 0, 1, 0); // Up
	return turtle;
}

mat3 transposeMat3(mat3 mat) {
	mat3 result = mat3();
	result[0] = vec3(mat[0].x, mat[1].x, mat[2].x);
	result[1] = vec3(mat[0].y, mat[1].y, mat[2].y);
	result[2] = vec3(mat[0].z, mat[1].z, mat[2].z);
	return result;
}

mat4 transposeMat4(mat4 mat) {
	mat4 result = mat4();
	result[0] = vec4(mat[0].x, mat[1].x, mat[2].x, mat[3].x);
	result[1] = vec4(mat[0].y, mat[1].y, mat[2].y, mat[3].y);
	result[2] = vec4(mat[0].z, mat[1].z, mat[2].z, mat[3].z);
	result[3] = vec4(mat[0].w, mat[1].w, mat[2].w, mat[3].w);
	return result;
}

mat3 GetAxes(mat4 turtle) {
	mat3 result = mat3();
	result[0] = vec3(turtle[HEADING].x, turtle[LEFT].x, turtle[UP].x);
	result[1] = vec3(turtle[HEADING].y, turtle[LEFT].y, turtle[UP].y);
	result[2] = vec3(turtle[HEADING].z, turtle[LEFT].z, turtle[UP].z);
	return result;
}

mat4 SetAxes(mat4 turtle, mat3 axes) {
	mat3 axesT = transposeMat3(axes);
	turtle[HEADING] = vec4(axesT[0], 0);
	turtle[LEFT] = vec4(axesT[1], 0);
	turtle[UP] = vec4(axesT[2], 0);
	return turtle;
}

mat3 RotateU(GLfloat alpha) {
	GLfloat alphaR = alpha * DegreesToRadians;
	mat3 result = mat3();
	result[0] = vec3(cos(alphaR), sin(alphaR), 0);
	result[1] = vec3(-sin(alphaR), cos(alphaR), 0);
	result[2] = vec3(0, 0, 1);
	return result;
}

mat4 RotateU(mat4 turtle, GLfloat alpha) {
	mat3 newAxes = GetAxes(turtle) * RotateU(alpha);
	return SetAxes(turtle, newAxes);
}

mat3 RotateL(GLfloat alpha) {
	GLfloat alphaR = alpha * DegreesToRadians;
	mat3 result = mat3();
	result[0] = vec3(cos(alphaR), 0, -sin(alphaR));
	result[1] = vec3(0, 1, 0);
	result[2] = vec3(sin(alphaR), 0, cos(alphaR));
	return result;
}

mat4 RotateL(mat4 turtle, GLfloat alpha) {
	mat3 newAxes = GetAxes(turtle) * RotateL(alpha);
	return SetAxes(turtle, newAxes);
}

mat3 RotateH(GLfloat alpha) {
	GLfloat alphaR = alpha * DegreesToRadians;
	mat3 result = mat3();
	result[0] = vec3(1, 0, 0);
	result[1] = vec3(0, cos(alphaR), -sin(alphaR));
	result[2] = vec3(0, sin(alphaR), cos(alphaR));
	return result;
}

mat4 RotateH(mat4 turtle, GLfloat alpha) {
	mat3 newAxes = GetAxes(turtle) * RotateH(alpha);
	return SetAxes(turtle, newAxes);
}

#endif
