//
//  OpenGL 3.2 Template for Mac
//  Ben Visness
//

// Suppress GLUT deprecated warnings on Mac
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <GL/Angel.h>
#include "GLMiddleman.h"
#include "Camera.h"
#include <math.h>
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "ILUT.lib")
#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "ILU.lib")

#include <stdio.h>

//store window width and height
int ww = 1000, wh = 700;

int mouseX = 0;
int mouseY = 0;
int mouseXDelta = 0;
int mouseYDelta = 0;
bool mouseLeftPressed = false;
bool mouseRightPressed = false;

GLMiddleman* middleman;
Camera mainCam;

void display(void)
{
	/*clear all pixels*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
#ifdef __APPLE__
		printf("%s\n", gluErrorString(error));
#else
		//        printf("%s\n", glewErrorString(error));
#endif
	}

	// Draw the scene

	glutSwapBuffers();
}

void mouseClick(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		mouseLeftPressed = (state == GLUT_DOWN);
	}
	if (button == GLUT_RIGHT_BUTTON) {
		mouseRightPressed = (state == GLUT_DOWN);
	}
}

void mouseMove(int x, int y) {
	y = -y;
	mouseXDelta = x - mouseX;
	mouseYDelta = y - mouseY;
	mouseX = x;
	mouseY = y;

	// Custom code here
}

void keyboard(unsigned char key, int x, int y) {
	if (key == 27) {
		exit(0);
	}
	
	// Custom keyboard code
}

void keyboardUp(unsigned char key, int x, int y) {

}

void special(int key, int x, int y) {

}

void specialUp(int key, int x, int y) {

}

void init() {
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_MULTISAMPLE);

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	middleman = new GLMiddleman();
}

void reshape(int width, int height){
	ww = width;
	wh = height;

	mat4 newProjectionMatrix = Perspective(mainCam.fov, (float)ww / wh, mainCam.nearClip, mainCam.farClip);
	middleman->updateProjectionMatrix(newProjectionMatrix);

	glViewport(0, 0, width, height);
}

void timer(GLint v) {
	// Custom timer code here

	glutPostRedisplay();
	glutTimerFunc(1000 / v, timer, v);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(ww, wh);
#ifdef __APPLE__
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
#else
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
#endif
	glutCreateWindow("OpenGL 3.2");

#ifndef __APPLE__
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	init();

	glutDisplayFunc(display);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove);
	glutPassiveMotionFunc(mouseMove);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(special);
	glutSpecialUpFunc(specialUp);
	glutReshapeFunc(reshape);
	glutTimerFunc(0, timer, 60);

	glutMainLoop();
	return 0;
}
