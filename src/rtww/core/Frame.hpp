#pragma once

#include "core/World.hpp"
#include "core/Camera.hpp"

#include <thread>
#include <Windows.h>
#include <sstream>
#include "core/ThreadPool.h"
#include "core/File.hpp"
#include "core/Image.hpp"
#include "gif.h"

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
	FrameRenderer(const char* name, const fs::path& path, uint32_t frameRate, uint16_t count) 
		: path(path), name(name), frameRate(frameRate){
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
	std::string name;
	uint32_t frameRate;
	uint16_t threadCount;
	std::vector<std::shared_ptr<FrameSettings>> frames;
	std::shared_ptr<ThreadPool> pool;
};
std::mutex consoleMutex;

void FrameRenderer::Render(std::vector<Color>(*Draw)(int, std::shared_ptr<FrameSettings>), uint32_t startIndex, uint32_t endIndex) {
	endIndex = endIndex == 0 ? frames.size() : endIndex;
	DWORD totalTime = 0;

	auto gifRate = 100.0f / frameRate;
	GifWriter writer = {};
	GifBegin(&writer, (path / (name + ".gif")).string().c_str(), frames[startIndex]->imageWidth, frames[startIndex]->imageHeight, gifRate, 8, true);
	
	for (uint32_t index = startIndex; index < endIndex; ++index) {
		pool = std::make_shared<ThreadPool>(threadCount);
		consoleMutex.lock();
		std::cerr << "The frame " << index + 1 << " starts rendering.\n" << std::flush;
		consoleMutex.unlock();
		DWORD start = ::GetTickCount();
		std::stringstream ss;

		std::vector<std::vector<Color>> pixels(frames[index]->imageHeight, std::vector<Color>(frames[index]->imageWidth, Color(0, 0, 0)));
		std::vector<std::future<std::vector<Color>>> result(frames[index]->imageHeight);
		for (int j = frames[index]->imageHeight - 1; j >= 0; --j) {
			result[j] = pool->enqueue(Draw, j, frames[index]);
		}
		for (int j = frames[index]->imageHeight - 1; j >= 0; --j) {
			pixels[j] = result[j].get();
		}
		uint8_t* data = new uint8_t[frames[index]->imageHeight*frames[index]->imageWidth*4];
		for (int j = frames[index]->imageHeight - 1; j >= 0; --j) {
			for (int i = 0; i < frames[index]->imageWidth; ++i) {
				auto c = ConvertColor(pixels[j][i], frames[index]->samplesPerPixel);
				ss << (char)c.x << (char)c.y << (char)c.z;
				auto idx = ((frames[index]->imageHeight -1 -j)*frames[index]->imageHeight + i) * 4;
				data[idx] = (uint8_t)c.x;
				data[idx+1] = (uint8_t)c.y;
				data[idx+2] = (uint8_t)c.z;
				data[idx+3] = (uint8_t)256;
			}
		}
		auto str = ss.str();
		auto filePath = path / (name + "-Frame" + std::to_string(index) + ".jpg");
		Image::Write2JPG(filePath.string().c_str(), str.c_str(), frames[index]->imageWidth, frames[index]->imageHeight, 100);
		GifWriteFrame(&writer, data, frames[index]->imageWidth, frames[index]->imageHeight, gifRate, 8, true);
		delete[] data;
		auto time = (::GetTickCount() - start) / 1000.0f;
		totalTime += time; consoleMutex.lock();
		std::cerr << "The frame " << index + 1 << " rendering is complete.Total time: " << time << "s\n" << std::flush;
		consoleMutex.unlock();
	}

	GifEnd(&writer);
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