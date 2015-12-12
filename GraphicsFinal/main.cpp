//
//  OpenGL 3.2 Template for Mac
//  Ben Visness
//

// Suppress GLUT deprecated warnings on Mac
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <GL/Angel.h>
#include "Camera.h"
#include "VisnessInitShader.h"
#include "VisnessUtil.h"
#include "Grammar.h"
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

Camera mainCam;

#define NUM_VERTICES 360
#define WORK_GROUP_SIZE 6

// References to shader programs
GLuint progRender;
GLuint progCompute;

// References to buffers and objects
GLuint vao;
GLuint vboPosition;
GLuint vboColor;
GLuint ssboPosition;
const GLuint ssboBindingIndex = 42;

// References to shader attributes
GLuint vPosition;
GLuint vAmbientDiffuseColor;
GLuint uModelView;
GLuint uProjection;

// Arrays of vertex data
Vector4 vertexPositions[NUM_VERTICES];
Vector4 vertexColors[NUM_VERTICES];

Grammar* grammar;

void display(void)
{
	/*clear all pixels*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
        printf("%s\n", gluErrorString(error));
	}

	// Draw the scene
	/*glBindBuffer(GL_ARRAY_BUFFER, vPositionBuffer);
	printf("Here's the error before: %s\n", gluErrorString(glGetError()));
	glVertexPointer(4, GL_FLOAT, 0, points);
	printf("Here's the error after: %s\n", gluErrorString(glGetError()));
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_POINTS, 0, NUM_VERTICES);
	glDisableClientState(GL_VERTEX_ARRAY);*/

	glUseProgram(progRender);
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, NUM_VERTICES);

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
	if (key == 'c') {
		printf("Trying to compute...\n");
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssboBindingIndex, vboPosition);
		glUseProgram(progCompute);
		glDispatchCompute(NUM_VERTICES / WORK_GROUP_SIZE, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
	else if (key == 'd') {
		std::string derivation = grammar->runDerivation();
		printf("%s\n", derivation.c_str());
	}
}

void keyboardUp(unsigned char key, int x, int y) {

}

void special(int key, int x, int y) {

}

void specialUp(int key, int x, int y) {

}

void initObjects() {
	mainCam = Camera();
	for (int i = 0; i < NUM_VERTICES; i++)
	{
		vertexPositions[i] = Vector4(randRange(-1, 1), randRange(-1, 1), randRange(-1, 1), 1);
		vertexColors[i] = Vector4(1, 1, 1, 1);
	}

	std::vector<Production> productions = {
		{ 'A', "ABA" },
	};
	grammar = new Grammar("BAB", productions);
}

void initShaders() {
	progRender = InitShader("vshader-simple.glsl", "fshader-simple.glsl");
	progCompute = InitComputeShader("cshader-circlevertices.glsl");
}

void initBuffers() {
	// Create vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create standard vbo and buffer data
	glGenBuffers(1, &vboPosition);
	glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat)* NUM_VERTICES, vertexPositions, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(progRender, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &vboColor);
	glBindBuffer(GL_ARRAY_BUFFER, vboColor);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat)* NUM_VERTICES, vertexColors, GL_STATIC_DRAW);
	vAmbientDiffuseColor = glGetAttribLocation(progRender, "vAmbientDiffuseColor");
	glEnableVertexAttribArray(vAmbientDiffuseColor);
	glVertexAttribPointer(vAmbientDiffuseColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// Get uniform references
	uModelView = glGetUniformLocation(progRender, "uModelView");
	uProjection = glGetUniformLocation(progRender, "uProjection");

	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;

	// Bind our SSBO to the same place in memory
	GLuint ssboBlockIndex = glGetProgramResourceIndex(progCompute, GL_SHADER_STORAGE_BLOCK, "SSBO");
	glShaderStorageBlockBinding(progCompute, ssboBlockIndex, ssboBindingIndex);
	
	
	glGenBuffers(1, &ssboPosition);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPosition);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_VERTICES * sizeof(Vector4), NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, vboPosition, ssboPosition);





	/*vertexPositions = (Vector4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, NUM_VERTICES * sizeof(Vector4), bufMask);
	for (int i = 0; i < NUM_VERTICES; i++)
	{
		points[i] = Vector4(0, 0, 0, 1);
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, vPositionBuffer);
	glUseProgram(progCompute);
	glDispatchCompute(NUM_VERTICES / WORK_GROUP_SIZE, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	*/
}

void init() {
	glClearColor(0, 0, 0, 0);

	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_MULTISAMPLE);

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	initObjects();
	initShaders();
	glUseProgram(progRender);
	initBuffers();

	mat4 newModelView = LookAt(Vector4(0, 0, 3, 0), Vector4(0, 0, 0, 0), Vector4(0, 1, 0, 0));
	glUniformMatrix4fv(uModelView, 1, GL_TRUE, newModelView);
}

void reshape(int width, int height){
	ww = width;
	wh = height;

	mat4 newProjectionMatrix = Perspective(mainCam.fov, (float)ww / wh, mainCam.nearClip, mainCam.farClip);
	glUniformMatrix4fv(uProjection, 1, GL_TRUE, newProjectionMatrix);

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
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(ww, wh);
#ifdef __APPLE__
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
#else
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
#endif
	glutCreateWindow("Ben Visness Graphics Final");

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
