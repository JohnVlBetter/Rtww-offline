#pragma once

#include "shape/Sphere.hpp"
#include "shape/Medium.hpp"
#include "shape/Box.hpp"
#include "shape/Rectangle.hpp"
#include "shape/FlipFace.hpp"
#include "core/World.hpp"
#include "core/Camera.hpp"
#include "core/BVH.hpp"
#include "core/Texture.hpp"
#include "core/PDF.hpp"

#include <thread>
#include <Windows.h>
#include "core/ThreadPool.h"

struct FrameSettings {
	std::shared_ptr<Camera> camera;
	std::shared_ptr<ShapesSet> objects;
	std::shared_ptr<ShapesSet> lights;
	Color backgroundColor;

	uint16_t imageWidth, imageHeight;
	Float rayTracingDepth;
	uint32_t samplesPerPixel;
};
/*
uint16_t imageWidth = 1024;
	Float aspectRatio = 1.0f;
	uint16_t imageHeight = static_cast<int>(imageWidth / aspectRatio);

	Vector3f vup(0, 1, 0);
	Point3f lookfrom = Point3f(278, 278, -800);
	Point3f lookat = Point3f(278, 278, 0);
	auto vfov = 40.0;
	auto dist2Focus = 10.0f;
	auto aperture = 0.0;

	auto lights = std::make_shared<ShapesSet>();
	lights->Add(std::make_shared<RectangleXZ>(200, 356, 214, 345, 554, std::shared_ptr<Material>()));
	lights->Add(std::make_shared<Sphere>(Point3f(275, 75, 190), 75, std::shared_ptr<Material>()));

	FrameRenderer renderer("CornellBox");
	auto camera = std::make_shared<Camera>(lookfrom, lookat, vup, vfov, aspectRatio, aperture, dist2Focus);
	auto settings = std::make_shared<FrameSettings>();
	settings->backgroundColor = Color(0, 0, 0);
	settings->camera = camera;
	settings->objects = std::make_shared<ShapesSet>(CornellBox());
	settings->lights = lights;
	settings->imageWidth = imageWidth;
	settings->imageHeight = imageHeight;
	settings->rayTracingDepth = 50;
	settings->samplesPerPixel = 100;

	renderer.AddFrame(settings);
	renderer.Render(0, 1);
*/

class FrameRenderer {
public:
	FrameRenderer(const char* name) : name(name) {}

	void AddFrame(std::shared_ptr<FrameSettings> frame) { frames.emplace_back(frame); }
	void Render(std::vector<Color>(*Draw)(int), uint32_t startIndex = 0, uint32_t endIndex = 0);

public:
	std::string name;
	std::vector< std::shared_ptr<FrameSettings> > frames;
};

void FrameRenderer::Render(std::vector<Color>(*Draw)(int), uint32_t startIndex, uint32_t endIndex) {
	endIndex = endIndex == 0 ? frames.size() : endIndex;

	for (int index = startIndex; index < endIndex; ++index) {
		std::cerr << "The frame " << index + 1 << " starts rendering.\n" << std::flush;
		DWORD start = ::GetTickCount();

		std::cout << "P3\n" << frames[index]->imageWidth << ' ' << frames[index]->imageHeight << "\n255\n";
		ThreadPool pool(10);
		std::vector<std::vector<Color>> pixels(frames[index]->imageWidth, std::vector<Color>(frames[index]->imageHeight, Color(0, 0, 0)));
		std::vector<std::future<std::vector<Color>>> result(frames[index]->imageHeight);
		for (int j = frames[index]->imageHeight - 1; j >= 0; --j) {
			result[j] = pool.enqueue(Draw, j);
		}
		for (int j = frames[index]->imageHeight - 1; j >= 0; --j) {
			pixels[j] = result[j].get();
		}
		for (int j = frames[index]->imageHeight - 1; j >= 0; --j) {
			for (int i = 0; i < frames[index]->imageWidth; ++i) {
				std::cout << pixels[j][i];
			}
		}

		std::cerr << "The frame " << index + 1 << " rendering is complete.\n" << std::flush;
		std::cerr << "Total time: " << (::GetTickCount() - start) / 1000.0f << "s\n" << std::flush;
	}
}