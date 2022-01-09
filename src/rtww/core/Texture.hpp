#pragma once
#include "Core.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Texture {
public:
	virtual Color Value(Float u, Float v, const Point3f& p) const = 0;
};

class SolidColorTexture : public Texture {
public:
	SolidColorTexture() {}
	SolidColorTexture(Color c) : color(c) {}

	SolidColorTexture(Float red, Float green, Float blue) : color(Color(red, green, blue)) {}

	virtual Color Value(Float u, Float v, const Point3f& p) const override {
		return color;
	}

private:
	Color color;
};

class CheckerTexture : public Texture {
public:
	CheckerTexture() {}

	CheckerTexture(std::shared_ptr<Texture> _even, std::shared_ptr<Texture> _odd) : even(_even), odd(_odd) {}

	CheckerTexture(Color c1, Color c2)
		: even(std::make_shared<SolidColorTexture>(c1)), odd(std::make_shared<SolidColorTexture>(c2)) {}

	virtual Color Value(double u, double v, const Point3f& p) const override {
		auto sines = sin(10 * p.x) * sin(10 * p.y) * sin(10 * p.z);
		if (sines < 0)
			return odd->Value(u, v, p);
		else
			return even->Value(u, v, p);
	}

public:
	std::shared_ptr<Texture> odd;
	std::shared_ptr<Texture> even;
};

class ImageTexture : public Texture {
public:
	const static int bytesPerPixel = 3;
	ImageTexture():data(nullptr),width(0),height(0),bytesPerScanLine(0) {}
	ImageTexture(const char* fileName){
		auto componentsPerPixel = bytesPerPixel;
		data = stbi_load(fileName, &width, &height, &componentsPerPixel, componentsPerPixel);
		if (!data) {
			std::cerr << "ERROR: Could not load texture image file '" << fileName << "'.\n";
			height = width = 0;
		}
		bytesPerScanLine = bytesPerPixel * width;
	}

	~ImageTexture() { STBI_FREE(data); }

	virtual Color Value(double u, double v, const Point3f& p) const override {
		if (data == nullptr) return Color(0.92f, 0.33f, 0.9f);

		u = Clamp<Float>(u, 0.0, 1.0);
		v = 1.0f - Clamp<Float>(v, 0.0, 1.0);

		auto i = static_cast<int>(u * width);
		auto j = static_cast<int>(v * height);
		if (i >= width) i = width - 1;
		if (j >= height) j = height - 1;

		const auto colorScale = 1.0 / 256.0;
		auto pixel = data + j * bytesPerScanLine + i * bytesPerPixel;
		return Color(colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]);
	}

private:
	uint8_t *data;
	int width, height;
	int bytesPerScanLine;
};