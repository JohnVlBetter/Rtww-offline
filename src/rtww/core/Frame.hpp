#pragma once

#include "core/World.hpp"
#include "core/Camera.hpp"

#include <thread>
#include <Windows.h>
#include <sstream>
#include "core/ThreadPool.h"
#include "core/File.hpp"

struct FrameSettings {
	std::shared_ptr<Camera> camera;
	std::shared_ptr<ShapesSet> objects;
	std::shared_ptr<ShapesSet> lights;
	Color backgroundColor;

	uint16_t imageWidth, imageHeight;
	uint32_t rayTracingDepth, samplesPerPixel;

	void SetImageOptions(uint16_t width, uint16_t height) {
		imageWidth = width;
		imageHeight = height;
	}

	void SetScene(std::shared_ptr<Camera> cam, std::shared_ptr<ShapesSet> obj, std::shared_ptr<ShapesSet> lig, const Color& bgColor) {
		camera = cam;
		objects = obj;
		lights = lig;
		backgroundColor = bgColor;
	}

	void SetRayTraceOptions(uint32_t depth, uint32_t samples) {
		rayTracingDepth = depth;
		samplesPerPixel = samples;
	}
};

class FrameRenderer {
public:
	FrameRenderer(const fs::path& path, uint16_t count) : path(path){
		if (count <= 0) {
			auto num = std::thread::hardware_concurrency();
			threadCount = num > 0 ? num : 1;
		}
		else threadCount = count;
	}

	void AddFrame(std::shared_ptr<FrameSettings> frame) { frames.emplace_back(frame); }
	void Render(std::vector<Color>(*Draw)(int, std::shared_ptr<FrameSettings>), uint32_t startIndex = 0, uint32_t endIndex = 0);

public:
	fs::path path;
	uint16_t threadCount;
	std::vector<std::shared_ptr<FrameSettings>> frames;
	std::shared_ptr<ThreadPool> pool;
};
std::mutex consoleMutex;

void FrameRenderer::Render(std::vector<Color>(*Draw)(int, std::shared_ptr<FrameSettings>), uint32_t startIndex, uint32_t endIndex) {
	endIndex = endIndex == 0 ? frames.size() : endIndex;
	DWORD totalTime = 0;
	for (uint32_t index = startIndex; index < endIndex; ++index) {
		pool = std::make_shared<ThreadPool>(threadCount);
		consoleMutex.lock();
		std::cerr << "The frame " << index + 1 << " starts rendering.\n" << std::flush;
		consoleMutex.unlock();
		DWORD start = ::GetTickCount();
		std::stringstream ss;

		ss << "P3\n" << frames[index]->imageWidth << ' ' << frames[index]->imageHeight << "\n255\n";
		
		std::vector<std::vector<Color>> pixels(frames[index]->imageWidth, std::vector<Color>(frames[index]->imageHeight, Color(0, 0, 0)));
		std::vector<std::future<std::vector<Color>>> result(frames[index]->imageHeight);
		for (int j = frames[index]->imageHeight - 1; j >= 0; --j) {
			result[j] = pool->enqueue(Draw, j, frames[index]);
		}
		for (int j = frames[index]->imageHeight - 1; j >= 0; --j) {
			pixels[j] = result[j].get();
		}
		for (int j = frames[index]->imageHeight - 1; j >= 0; --j) {
			for (int i = 0; i < frames[index]->imageWidth; ++i) {
				ss << pixels[j][i];
			}
		}
		auto str = ss.str();
		auto filePath = path / (std::to_string(index) + ".ppm");
		RTWWFile::Write2File(filePath.string().c_str(), str.c_str(), str.length());
		auto time = (::GetTickCount() - start) / 1000.0f;
		totalTime += time; consoleMutex.lock();
		std::cerr << "The frame " << index + 1 << " rendering is complete.Total time: " << time << "s\n" << std::flush;
		consoleMutex.unlock();
	}
	std::stringstream timeStr;
	int hours = std::floor(totalTime / 3600);
	int minutes = std::floor(totalTime % 3600 / 60);
	int seconds = std::floor(totalTime % 3600 % 60);
	if (hours > 0) timeStr << std::to_string(hours) << "h";
	if (hours > 0 || minutes > 0) timeStr << std::to_string(minutes) << "m";
	timeStr << std::to_string(seconds) << "s";
	consoleMutex.lock();
	std::cerr << "\nAll rendering is completed, the total time is " << timeStr.str() << "\n" << std::flush;
	consoleMutex.unlock();
}