//
//  Image.cpp
//  Assignment05
//
//  Created by Benjamin Visness on 11/18/15.
//  Copyright (c) 2015 Benjamin Visness. All rights reserved.
//

#include "Texture2D.h"

Texture2D::Texture2D(ImageInfo image, GLenum magFilter, GLenum minFilter) {
	ilBindImage(image.id);
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
	glTexImage2D(GL_TEXTURE_2D, 0, image.bpp, image.width, image.height, 0, image.format, image.type, image.data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

GLuint Texture2D::getId() {
	return texId;
}
