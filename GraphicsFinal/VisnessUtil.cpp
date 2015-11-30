//
//  VisnessUtil.cpp
//  Assignment01
//
//  Created by Benjamin Visness on 9/9/15.
//  Copyright (c) 2015 Benjamin Visness. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <sstream>

#include "VisnessUtil.h"

GLfloat randRange(GLfloat min, GLfloat max) {
    return ((GLfloat)rand() / RAND_MAX) * (max - min) + min;
}

GLfloat clamp(GLfloat n, GLfloat min, GLfloat max) {
    if (n < min) {
        return min;
    }
    if (n > max) {
        return max;
    }
    
    return n;
}

void warnWithMessage(const char* msg) {
    printf("WARNING: %s\n", msg);
}

void warnWithMessage(std::string msg) {
    warnWithMessage(msg.c_str());
}

void abortWithMessage(const char* msg) {
    printf("ERROR: %s\n", msg);
    abort();
}

void abortWithMessage(std::string msg) {
    abortWithMessage(msg.c_str());
}

bool operator==(const Vector2& lhs, const Vector2& rhs) {
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

bool operator==(const Vector3& lhs, const Vector3& rhs) {
    return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
}

bool operator==(const Vector4& lhs, const Vector4& rhs) {
    return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z) && (lhs.w == rhs.w);
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    bool containsDelimeter = false;
    for (int i = 0; (unsigned int)i < s.size(); i++) {
        if (s[i] == delim) {
            containsDelimeter = true;
            break;
        }
    }
    
    if (!containsDelimeter) {
        std::vector<std::string> result;
        result.push_back(s);
        return result;
    }
    
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}
