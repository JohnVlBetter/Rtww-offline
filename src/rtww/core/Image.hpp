#pragma once

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "core/File.hpp"

#include <sstream>

class Image {
public:
	static void Write2JPG(const char* filePath, const char *data, int width, int height, int quality) {
		stbi_write_jpg(filePath, width, height, 3, data, quality);
	}
};