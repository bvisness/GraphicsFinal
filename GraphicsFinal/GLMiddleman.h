//
//  GLMiddleman.h
//  Assignment02
//
//  Created by Benjamin Visness on 9/29/15.
//  Copyright (c) 2015 Benjamin Visness. All rights reserved.
//

#ifndef __Assignment02__GLMiddleman__
#define __Assignment02__GLMiddleman__

#include "VisnessGL.h"
#include "VisnessUtil.h"
#include "Material.h"

#define MAX_LIGHTS 10
#define MAX_TEXTURES (GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1)

#define HAS_TEX_DIFFUSE 1
#define HAS_TEX_SPEC 2
#define HAS_TEX_NORM 4

/**
 * A class that handles shipping data over to
 * the graphics card using OpenGL methods.
 */
class GLMiddleman {
private:
	int numRegisteredLights = 0;
public:
    GLMiddleman();
    GLuint program;
    GLuint model_view;
    GLuint projection;
    GLuint vPosition;
    GLuint vNormal;
	GLuint vTangent;
	GLuint vBitangentSign;
	GLuint vTexCoord;
    GLuint vAmbientDiffuseColor;
    GLfloat vDiffuseAmount;
    GLfloat vSpecularAmount;
    GLfloat vSpecularExponent;
    GLuint uAmbientLight;
    GLuint uLightType;
    GLuint uLightColor;
    GLuint uLightPosition;
    GLuint uLightDirection;
	GLuint uLightSpotAngleCos;
	GLuint uTextureMode;
	GLuint uDiffuseTexture;
	GLuint uSpecularTexture;
	GLuint uNormalMap;

	const int diffuseTextureUnit = 0;
	const int specTextureUnit = 1;
	const int normTextureUnit = 2;

	Vector4 lightPositions[MAX_LIGHTS];
	Vector4 lightDirections[MAX_LIGHTS];
	Vector4 lightColors[MAX_LIGHTS];
	GLint lightTypes[MAX_LIGHTS];
	GLfloat lightSpotAngleCosines[MAX_LIGHTS];
	GLuint textureIds[MAX_TEXTURES];
    
    void updateProjectionMatrix(mat4 newMatrix);
    void updateModelViewMatrix(mat4 newMatrix);

	// If you modify this, make sure to update the following:
	// - VBO_COUNT in RenderedGameObject
	// - RenderedGameObject::customInitGameObject()
	// - Getter methods in RenderedGameObject.h
	// - Attribute ids in GLMiddleman.h (all the GLuints)
	// - Buffering methods in GLMiddleman.cpp
	struct ObjectInfo {
		GLuint vao = UINT_MAX;
		GLuint* vbo = nullptr;
		int unsigned numberOfVertices = 0;
		Vector4* vertices = nullptr;
		Vector3* vertexNormals = nullptr;
		Vector3* vertexTangents = nullptr;
		GLfloat* vertexBitangentSigns = nullptr;
		Vector2* vertexUVs = nullptr;
		Vector4* vertexColors = nullptr;
		Material material = Material();
	};

    void bufferObject(ObjectInfo object);

	int getLightId();
	void bufferLights();

	int genTextureUnit();
	void updateMaterialUniforms(Material material);
};

#endif /* defined(__Assignment02__GLMiddleman__) */
