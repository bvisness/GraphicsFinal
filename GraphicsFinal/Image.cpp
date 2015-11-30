//
//  Image.cpp
//  Assignment05
//
//  Created by Benjamin Visness on 11/18/15.
//  Copyright (c) 2015 Benjamin Visness. All rights reserved.
//

#include "Image.h"

#include "VisnessUtil.h"

bool Image::initialized = false;

void loadImageData(const char* filename) {
	ILboolean success;

	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION) {
		abortWithMessage("Wrong IL version");
	}

	success = ilLoadImage(filename);
	if (success) {
		//We need to figure out whether we have an alpha channel or not
		if (ilGetInteger(IL_IMAGE_BPP) == 3) {
			success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
		} else if (ilGetInteger(IL_IMAGE_BPP) == 4) {
			success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		} else {
			success = false;
		}

		if (!success) {
			abortWithMessage("Failed conversion to unsigned byte");
		}
	} else {
		abortWithMessage("Failed to load image " + std::string(filename));
	}
}

Image::Image(const char* filename) {
	if (!initialized) {
		ilInit();
		initialized = true;
	}

	info.id = ilGenImage();
	ilBindImage(info.id);
	loadImageData(filename);

	info.width = ilGetInteger(IL_IMAGE_WIDTH);
	info.height = ilGetInteger(IL_IMAGE_HEIGHT);
	info.bpp = ilGetInteger(IL_IMAGE_BPP);
	info.format = ilGetInteger(IL_IMAGE_FORMAT);
	info.type = ilGetInteger(IL_IMAGE_TYPE);
	info.data = ilGetData();
}

ImageInfo Image::getInfo() {
	return info;
}
