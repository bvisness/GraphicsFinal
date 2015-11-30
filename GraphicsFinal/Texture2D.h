//
//  Texture2D.h
//  Assignment05
//
//  Created by Benjamin Visness on 11/18/15.
//  Copyright (c) 2015 Benjamin Visness. All rights reserved.
//

#ifndef __Texture2D__
#define __Texture2D__

#include "VisnessGL.h"
#include "Image.h"

class Texture2D {
private:
	GLuint texId;
public:
	Texture2D(ImageInfo image, GLenum magFilter = GL_LINEAR_MIPMAP_LINEAR, GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR);

	GLuint getId();
};

#endif
