
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

mat3 GetAxes(mat4 turtle) {
	mat3 result = mat3();
	result[0] = vec3(turtle[HEADING].x, turtle[LEFT].x, turtle[UP].x);
	result[1] = vec3(turtle[HEADING].y, turtle[LEFT].y, turtle[UP].y);
	result[2] = vec3(turtle[HEADING].z, turtle[LEFT].z, turtle[UP].z);
	return result;
}

mat4 SetAxes(mat4 turtle, mat3 axes) {
	axes = transpose(axes);
	turtle[HEADING] = vec4(axes[0], 0);
	turtle[LEFT] = vec4(axes[1], 0);
	turtle[UP] = vec4(axes[2], 0);
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
