//
//  Image.h
//  Assignment05
//
//  Created by Benjamin Visness on 11/18/15.
//  Copyright (c) 2015 Benjamin Visness. All rights reserved.
//

#ifndef __Image__
#define __Image__

#include "VisnessGL.h"
#include <IL/il.h>

struct ImageInfo {
	ILuint id;
	ILint bpp;
	ILint format;
	ILint type;
	int width;
	int height;
	ILubyte* data;
};

class Image {
private:
	static bool initialized;
	ImageInfo info;
public:
	Image(const char* filename);
	ImageInfo getInfo();
};

#endif
