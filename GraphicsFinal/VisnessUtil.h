//
//  VisnessUtil.h
//  Assignment01
//
//  Created by Benjamin Visness on 9/9/15.
//  Copyright (c) 2015 Benjamin Visness. All rights reserved.
//

#ifndef __VISNESSUTIL_H__
#define __VISNESSUTIL_H__

#include <string>
#include <vector>

#include "VisnessGL.h"

#ifdef __APPLE__
#include "Angel.h"
#else
#include <GL/Angel.h>
#endif

#ifndef DEG_TO_RAD
#define DEG_TO_RAD 0.0174532925
#endif

#ifndef RAD_TO_DEG
#define RAD_TO_DEG 57.2957795
#endif

typedef mat2 Matrix2;
typedef vec2 Vector2;
typedef vec2 Point2;

typedef mat3 Matrix3;
typedef vec3 Vector3;
typedef vec3 Point3;

typedef mat4 Matrix4;
typedef vec4 Vector4;

bool operator==(const Vector2& lhs, const Vector2& rhs);
bool operator==(const Vector3& lhs, const Vector3& rhs);
bool operator==(const Vector4& lhs, const Vector4& rhs);

GLfloat randRange(GLfloat min, GLfloat max);

GLfloat clamp(GLfloat n, GLfloat min, GLfloat max);

void warnWithMessage(const char* msg);
void warnWithMessage(std::string msg);
void abortWithMessage(const char* msg);
void abortWithMessage(std::string msg);

std::vector<std::string> split(const std::string &s, char delim);

#endif
