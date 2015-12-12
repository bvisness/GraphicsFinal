
#ifndef GraphicsFinal_Camera_h
#define GraphicsFinal_Camera_h

#include "VisnessGL.h"

struct Camera {
	GLfloat fov = 45;
	GLfloat nearClip = 0.01;
	GLfloat farClip = 500;
};

#endif
