//
//  Material.h
//  Assignment04
//
//  Created by Benjamin Visness on 10/20/15.
//  Copyright (c) 2015 Benjamin Visness. All rights reserved.
//

#ifndef Assignment04_Material_h
#define Assignment04_Material_h

#include "Texture2D.h"

struct Material {
	Texture2D* diffuseTexture = nullptr;
	Texture2D* specTexture = nullptr;
	Texture2D* normalMap = nullptr;

    GLfloat diffuseAmount = 1;
    GLfloat specularAmount = 0.5;
    GLfloat specularExponent = 10;
};

#endif
