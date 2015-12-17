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
#include <algorithm>
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

#define NUM_VERTICES 36
#define NUM_WORK_GROUPS 1

#define CHUNK_SIZE 64

// References to shader programs
GLuint progRender;
GLuint progCompute;
GLuint progCStep1;
GLuint progCStep3;
GLuint progCStep5;

// References to buffers and objects
GLuint vao;
GLuint vboPosition;
GLuint vboColor;
GLuint ssboInputString;
GLuint ssboInputStringStartsEnds;
GLuint ssboChunkDepths;
GLuint ssboDepthCounts;
GLuint ssboBucketOffsets;
GLuint ssboBucketSortArray;
GLuint ssboBucketStartIndices;
GLuint ssboACEveryOther;
GLuint ssboPosition;
const GLuint ssboBindingIndex = 42;
const GLuint cStep1BindingIndex = 43;

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

	mat4 newModelView = LookAt(Vector4(0, 0, 3, 0), Vector4(0, 0, 0, 0), Vector4(0, 1, 0, 0));
	newModelView = newModelView * RotateY(mouseX);
	glUniformMatrix4fv(uModelView, 1, GL_TRUE, newModelView);

	glUseProgram(progRender);
	glBindVertexArray(vao);
	glDrawArrays(GL_LINES, 0, NUM_VERTICES);

	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ssboACEveryOther);
	GLuint counterVal = *(GLuint *)glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_READ_ONLY);
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
	//printf("uEveryOther value: %d\n", counterVal);

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

void prepareString(std::string derivation) {
	// Find the max depth of the string
	int maxDepth = 0;
	int depth = 0;
	for (int i = 0; i < derivation.length(); i++) {
		if (derivation[i] == '[') {
			depth++;
		}
		else if (derivation[i] == ']') {
			depth--;
		}

		if (depth > maxDepth) {
			maxDepth = depth;
		}
	}

	// Calculate the number of chunks necessary to process this string
	int numChunks = ceil(derivation.length() / (double)CHUNK_SIZE);

	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;

	// Buffer over the new string
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboInputString);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLint)* derivation.length(), NULL, GL_STATIC_DRAW);
	GLint* chars = (GLint*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLint)* derivation.length(), bufMask);
	for (int i = 0; i < derivation.length(); i++) {
		chars[i] = (GLint)derivation[i];
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	// Buffer the string start/end indices for each chunk
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboInputStringStartsEnds);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec2)* numChunks, NULL, GL_STATIC_DRAW);
	vec2* indices = (vec2*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(vec2)* numChunks, bufMask);
	int stringChunkSize = std::min((int)derivation.length(), CHUNK_SIZE);
	for (int i = 0; i < numChunks; i++)
	{
		indices[i].x = stringChunkSize * i;
		indices[i].y = (stringChunkSize * i) + stringChunkSize - 1;
		if (i == numChunks - 1) {
			indices[i].y = derivation.length() - 1;
		}
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	// Prepare the chunk depths buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboChunkDepths);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLint)* numChunks, NULL, GL_STATIC_DRAW);

	// Run step 1
	glUseProgram(progCStep1);
	glDispatchCompute(numChunks, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	// Run step 2 (parallel scan of chunk depths)
	// We are still bound to ssboChunkDepths
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboChunkDepths);
	GLint* chunkDepths = (GLint *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	{
		int oldValueAtIMinus1 = chunkDepths[0];
		chunkDepths[0] = 0;
		for (int i = 1; i < numChunks; i++) {
			int oldValueAtI = chunkDepths[i];
			chunkDepths[i] = chunkDepths[i - 1] + oldValueAtIMinus1;
			oldValueAtIMinus1 = oldValueAtI;
		}
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboChunkDepths);
	GLint* chunkStartDepths = (GLint *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	for (int i = 0; i < numChunks; i++) {
		printf("Chunk start depth %d: %d\n", i, chunkStartDepths[i]);
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	// Allocate the depth count and bucket offset arrays
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboDepthCounts);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLint)* numChunks * (maxDepth + 1), NULL, GL_STATIC_DRAW);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32I, GL_RED_INTEGER, GL_INT, NULL);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboBucketOffsets);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLint)* numChunks * (maxDepth + 1), NULL, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboBucketStartIndices);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLint)* (maxDepth + 1), NULL, GL_STATIC_DRAW);

	// Run step 3
	glUseProgram(progCStep3);
	glDispatchCompute(numChunks, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	// Run step 4 (scans of depth counts)
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboDepthCounts);
	GLint* depthCounts = (GLint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboBucketOffsets);
	GLint* bucketOffsets = (GLint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboBucketStartIndices);
	GLint* bucketStartIndices = (GLint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	for (int j = 0; j <= maxDepth; j++) {
		bucketOffsets[(j * numChunks) + 0] = 0;
		for (int i = 1; i < numChunks; i++) {
			bucketOffsets[(j * numChunks) + i] = bucketOffsets[(j * numChunks) + (i - 1)] + depthCounts[(j * numChunks) + (i - 1)];
		}
		for (int i = 0; i < numChunks; i++) {
			printf("        Pushes & pops at depth %d chunk %d: %d\n", j, i, depthCounts[(j * numChunks) + i]);
		}
		for (int i = 0; i < numChunks; i++) {
			printf("Scanned pushes & pops at depth %d chunk %d: %d\n", j, i, bucketOffsets[(j * numChunks) + i]);
		}
	}
	int bucketArrayLength = 0;
	bucketStartIndices[0] = 0;
	for (int j = 0; j <= maxDepth; j++) {
		bucketStartIndices[j] = bucketArrayLength;
		printf("Bucket start index %d: %d\n", j, bucketStartIndices[j]);

		int elementsInBucket = depthCounts[(j * numChunks) + (numChunks - 1)] + bucketOffsets[(j * numChunks) + (numChunks - 1)];
		bucketArrayLength += elementsInBucket;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboBucketOffsets);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboDepthCounts);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	// Allocate the bucket sort array
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboBucketSortArray);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLint)* bucketArrayLength, NULL, GL_STATIC_DRAW);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32I, GL_RED_INTEGER, GL_INT, NULL);

	// Run step 5 (fill bucket sort array)
	glUseProgram(progCStep5);
	glDispatchCompute(numChunks, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	// We are still bound to ssboBucketSortArray
	GLint* bucketSortArray = (GLint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	for (int i = 0; i < bucketArrayLength; i++) {
		int bucketDepth = 0;
		for (int j = 0; j <= maxDepth; j++) {
			if (i >= bucketStartIndices[j]) {
				bucketDepth = j;
			}
		}

		printf("Bucket item %d (depth %d): %d\n", i, bucketDepth, bucketSortArray[i]);
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
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
		glDispatchCompute(NUM_WORK_GROUPS, 1, 1);
		glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
	}
	else if (key == 'd') {
		std::string derivation = grammar->runDerivation();
		printf("%s\n", derivation.c_str());

		prepareString(derivation);
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
		{ 'F', "F[+F]F[-F]F" }
	};
	grammar = new Grammar("F", productions);
}

void initShaders() {
	progRender = InitShader("vshader-simple.glsl", "fshader-simple.glsl");
	progCompute = InitComputeShader("cshader-circlevertices.glsl");
	progCStep1 = InitComputeShader("cshader-step1.glsl");
	progCStep3 = InitComputeShader("cshader-step3.glsl");
	progCStep5 = InitComputeShader("cshader-step5.glsl");
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

	// Get our SSBO block index
	GLuint ssboBlockIndex = glGetProgramResourceIndex(progCompute, GL_SHADER_STORAGE_BLOCK, "SSBO");
	glShaderStorageBlockBinding(progCompute, ssboBlockIndex, ssboBindingIndex);

	glGenBuffers(1, &ssboInputString);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboInputString);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 30, ssboInputString);

	glGenBuffers(1, &ssboInputStringStartsEnds);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboInputStringStartsEnds);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 31, ssboInputStringStartsEnds);

	glGenBuffers(1, &ssboChunkDepths);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboChunkDepths);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 32, ssboChunkDepths);

	glGenBuffers(1, &ssboDepthCounts);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboDepthCounts);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 33, ssboDepthCounts);

	glGenBuffers(1, &ssboBucketOffsets);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboBucketOffsets);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 34, ssboBucketOffsets);

	glGenBuffers(1, &ssboBucketSortArray);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboBucketSortArray);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 35, ssboBucketSortArray);

	glGenBuffers(1, &ssboBucketStartIndices);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboBucketStartIndices);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 36, ssboBucketStartIndices);

	// Configure atomic counters
	glGenBuffers(1, &ssboACEveryOther);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ssboACEveryOther);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, ssboACEveryOther);
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
