#include "core/Core.hpp"
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
#include "core/ThreadPool.h"
#include "core/Frame.hpp"
#include "core/Transform.hpp"
#include "core/Model.hpp"
#include "shape/Triangle.hpp"
#include <thread>
#include <Windows.h>

Color RayColor(const Ray& r, const Color& background, std::shared_ptr <ShapesSet> world, std::shared_ptr<Shape> lights, int depth) {
	IntersectionRecord rec;

	if (depth <= 0)
		return Color(0, 0, 0);

	if (!world->Intersection(r, 0.001f, Infinity, rec)) {
		//auto t = 0.5*(r.direction.Normalize().y + 1.0);
		//return (1.0 - t)* Color(1.0, 1.0, 1.0) + t *  Color(0.5, 0.7, 1.0);
		return background;
	}

	ScatterRecord srec;
	Color emitted = rec.matPtr->Emitted(r, rec, rec.u, rec.v, rec.hitPoint);
	if (!rec.matPtr->Scatter(r, rec, srec))
		return emitted;

	if (srec.isSpecular) {
		return srec.attenuation
			* RayColor(srec.specularRay, background, world, lights, depth - 1);
	}

	auto light_ptr = std::make_shared<ShapePDF>(lights, rec.hitPoint);
	MixturePDF p(light_ptr, srec.pdfPtr);

	Ray scattered = Ray(rec.hitPoint, p.Generate(), r.time);
	auto pdf_val = p.Value(scattered.direction);

	return emitted
		+ srec.attenuation * rec.matPtr->ScatteringPDF(r, rec, scattered)
		* RayColor(scattered, background, world, lights, depth - 1) / pdf_val;
}

ShapesSet CornellBox2() {
	ShapesSet objects;

	auto red = std::make_shared<Lambertian>(Color(.65, .05, .05));
	auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
	auto pink = std::make_shared<Lambertian>(Color(1, .412, .706));
	auto blue = std::make_shared<Lambertian>(Color(.12, .15, .65));
	auto light = std::make_shared<DiffuseLight>(Color(15, 15, 15));

	objects.Add(std::make_shared<RectangleYZ>(std::make_shared<Transform>(
		Point3f(605, 277.5, 277.5), Vector3f(1, 555, 555), Vector3f()), blue));
	
	objects.Add(std::make_shared<RectangleYZ>(std::make_shared<Transform>(
		Point3f(-50, 277.5, 277.5), Vector3f(1, 555, 555), Vector3f()), red));
	
	objects.Add(std::make_shared<FlipFace>(std::make_shared<RectangleXZ>(std::make_shared<Transform>(
		Point3f(278, 554, 279.5), Vector3f(156, 1, 131), Vector3f()), light)));
	
	objects.Add(std::make_shared<RectangleXZ>(std::make_shared<Transform>(
		Point3f(277.5, 0, 177.5), Vector3f(655, 1, 755), Vector3f()), white));
	
	objects.Add(std::make_shared<RectangleXZ>(std::make_shared<Transform>(
		Point3f(277.5, 555, 277.5), Vector3f(655, 1, 555), Vector3f()), white));
	
	objects.Add(std::make_shared<RectangleXY>(std::make_shared<Transform>(
		Point3f(277.5, 277.5, 555), Vector3f(655, 555, 1), Vector3f()), white));

	std::shared_ptr<Material> aluminum = std::make_shared<Metal>(Color(0.8, 0.85, 0.88), 0.0);
	std::shared_ptr<Shape> box1 = std::make_shared<Box>(std::make_shared<Transform>(
		Point3f(85, 155, 290), Vector3f(165, 305, 110), Vector3f(0, 40, 0)), white);
	objects.Add(box1);
	
	std::shared_ptr<Shape> box2 = std::make_shared<Box>(std::make_shared<Transform>(
		Point3f(450, 130, 250), Vector3f(185, 260, 110), Vector3f(0, 50, 0)), pink);
	objects.Add(box2);

	auto glass = std::make_shared<Dielectric>(1.5);
	objects.Add(CreateSphere(Point3f(275, 75, 190), Vector3f(75, 75, 75), Vector3f(), glass));

	return objects;
}

ShapesSet Geometry() {
	ShapesSet objects;

	auto green = std::make_shared<Lambertian>(Color(0.6484375, 0.79, 0.6328125));
	auto whiteG = std::make_shared<Lambertian>(Color(.73, .83, .73));
	auto black = std::make_shared<Lambertian>(Color(0.02, 0.02, 0.02));
	auto light = std::make_shared<DiffuseLight>(Color(15, 15, 8));

	objects.Add(std::make_shared<RectangleYZ>(std::make_shared<Transform>(
		Point3f(-50, 277.5, 277.5), Vector3f(1, 655, 800), Vector3f()), green));

	objects.Add(std::make_shared<FlipFace>(std::make_shared<RectangleXZ>(std::make_shared<Transform>(
		Point3f(400, 700, 0), Vector3f(300, 1, 300), Vector3f(0, 0, 0)), light)));

	objects.Add(std::make_shared<RectangleXZ>(std::make_shared<Transform>(
		Point3f(277.5, 0, 177.5), Vector3f(655, 1, 755), Vector3f()), green));

	objects.Add(std::make_shared<RectangleXY>(std::make_shared<Transform>(
		Point3f(277.5, 277.5, 555), Vector3f(655, 655, 1), Vector3f()), green));

	std::shared_ptr<Shape> box1 = std::make_shared<Box>(std::make_shared<Transform>(
		Point3f(250, 10, 250), Vector3f(300, 20, 20), Vector3f(0, 0, 0)), whiteG);
	objects.Add(box1);

	std::shared_ptr<Shape> box2 = std::make_shared<Box>(std::make_shared<Transform>(
		Point3f(390, 135, 250), Vector3f(20, 300, 20), Vector3f(0, 0, 0)), whiteG);
	objects.Add(box2);

	std::shared_ptr<Shape> box3 = std::make_shared<Box>(std::make_shared<Transform>(
		Point3f(110, 135, 250), Vector3f(20, 300, 20), Vector3f(0, 0, 0)), whiteG);
	objects.Add(box3);

	std::shared_ptr<Shape> box4 = std::make_shared<Box>(std::make_shared<Transform>(
		Point3f(250, 290, 250), Vector3f(300, 20, 20), Vector3f(0, 0, 0)), whiteG);
	objects.Add(box4);

	std::shared_ptr<Shape> line1 = std::make_shared<Box>(std::make_shared<Transform>(
		Point3f(305, 220, 250), Vector3f(1, 160, 1), Vector3f(0, 0, 0)), black);
	objects.Add(line1);

	std::shared_ptr<Shape> line2 = std::make_shared<Box>(std::make_shared<Transform>(
		Point3f(195, 220, 250), Vector3f(1, 160, 1), Vector3f(0, 0, 0)), black);
	objects.Add(line2);

	auto glass = std::make_shared<Dielectric>(1.5);
	std::shared_ptr<Material> aluminum = std::make_shared<Metal>(Color(0.5, 0.5, 0.6), 0.0);
	objects.Add(CreateSphere(Point3f(195, 120, 250), Vector3f(40, 40, 40), Vector3f(), aluminum));
	objects.Add(CreateSphere(Point3f(305, 120, 250), Vector3f(40, 40, 40), Vector3f(), glass));
	
	return objects;
}

ShapesSet Triangles() {
	ShapesSet objects;

	auto green = std::make_shared<Lambertian>(Color(0.6484375, 0.79, 0.6328125));
	auto red = std::make_shared<Lambertian>(Color(1, 0, 0));
	auto blue = std::make_shared<Lambertian>(Color(0, 0, 1));
	auto realGreen = std::make_shared<Lambertian>(Color(0, 1, 0));
	auto whiteG = std::make_shared<Lambertian>(Color(.73, .83, .73));
	auto black = std::make_shared<Lambertian>(Color(0.02, 0.02, 0.02));
	auto yellow = std::make_shared<Lambertian>(Color(1, 1, 0));
	auto light = std::make_shared<DiffuseLight>(Color(15, 15, 8));
	auto aluminum = std::make_shared<Metal>(Color(0.5, 0.5, 0.6), 0.0);

	objects.Add(std::make_shared<RectangleYZ>(std::make_shared<Transform>(
		Point3f(-50, 277.5, 277.5), Vector3f(1, 800, 1000), Vector3f()), whiteG));

	objects.Add(std::make_shared<FlipFace>(std::make_shared<RectangleXZ>(std::make_shared<Transform>(
		Point3f(400, 700, 0), Vector3f(300, 1, 300), Vector3f(0, 0, 0)), light)));

	objects.Add(std::make_shared<RectangleXZ>(std::make_shared<Transform>(
		Point3f(277.5, 0, 177.5), Vector3f(900, 1, 755), Vector3f()), whiteG));

	objects.Add(std::make_shared<RectangleXY>(std::make_shared<Transform>(
		Point3f(277.5, 277.5, 555), Vector3f(900, 655, 1), Vector3f()), whiteG));

	return objects;
}

ShapesSet CornellBoxModel() {
	ShapesSet objects;
	auto light = std::make_shared<DiffuseLight>(Color(15, 15, 15));
	auto whiteG = std::make_shared<Lambertian>(Color(0.73, 0.83, .73));

	objects.Add(std::make_shared<Sphere>(std::make_shared<Transform>(
		Point3f(0, 900, 500), Vector3f(300, 300, 300), Vector3f(0, 0, 0)), light));
	/*objects.Add(std::make_shared<FlipFace>(std::make_shared<RectangleXZ>(std::make_shared<Transform>(
		Point3f(0, 899, 500), Vector3f(300, 1, 300), Vector3f(0, 0, 0)), light)));*/

	//objects.Add(std::make_shared<RectangleYZ>(std::make_shared<Transform>(
	//	Point3f(600, 450, 450), Vector3f(1, 900, 1200), Vector3f()), whiteG));
	//objects.Add(std::make_shared<RectangleYZ>(std::make_shared<Transform>(
	//	Point3f(-600, 450, 450), Vector3f(1, 900, 1200), Vector3f()), whiteG));
	//objects.Add(std::make_shared<RectangleXZ>(std::make_shared<Transform>(
	//	Point3f(0, 0, 0), Vector3f(1200, 1, 1200), Vector3f()), whiteG));
	//objects.Add(std::make_shared<RectangleXZ>(std::make_shared<Transform>(
	//	Point3f(0, 900, 0), Vector3f(1200, 1, 1200), Vector3f()), whiteG));
	//objects.Add(std::make_shared<RectangleXY>(std::make_shared<Transform>(
	//	Point3f(0, 450, 450), Vector3f(1200, 900, 1), Vector3f()), whiteG));

	Model model("D:/Workspace/CG/Repos/Rtww-offline/resources/models/usemtl-issue-68.obj",std::make_shared<Transform>(
		Point3f(0, 0, 650), Vector3f(80, 80, 80), Vector3f(0,10,0)));
	ShapesSet modelT;
	for (auto& mesh : model.meshes) {
		auto triangles = GetMeshTriangles(mesh);
		for (int i = 0; i < triangles.size(); ++i)
			modelT.Add(triangles[i]);
	}
	objects.Add(std::make_shared<BVHNode>(modelT, 0, 1));
	std::shared_ptr<Shape> box = std::make_shared<Box>(std::make_shared<Transform>(
		Point3f(0, -250, 650), Vector3f(500, 500, 500), Vector3f(0, 50, 0)), whiteG);
	objects.Add(box);
	return objects;
}

std::vector<Color> Draw(int index, std::shared_ptr<FrameSettings> settings) {
	std::vector<Color> t(settings->imageWidth, Color(0, 0, 0));
	for (int i = 0; i < settings->imageWidth; ++i) {
		Color pixelColor(0, 0, 0);
		for (uint32_t k = 0; k < settings->samplesPerPixel; ++k) {
			auto u = Float(i + Random<Float>()) / (settings->imageWidth - 1);
			auto v = Float(index + Random<Float>()) / (settings->imageHeight - 1);
			Ray r = settings->camera->GenerateRay(u, v);
			pixelColor += RayColor(r, settings->backgroundColor, settings->objects, settings->lights, settings->rayTracingDepth);
		}
		t[i] = pixelColor;
	}
	consoleMutex.lock();
	std::cerr << "Line " << index << " is done.\n";
	consoleMutex.unlock();
	return t;
}

int main(int argc, char** argv) {
	auto lights = std::make_shared<ShapesSet>();
	lights->Add(std::make_shared<Sphere>(std::make_shared<Transform>(
		Point3f(0, 900, 500), Vector3f(300, 300, 300), Vector3f(0, 0, 0)), std::shared_ptr<Material>()));
	fs::path imageParentPath("D:/Workspace/CG/Repos/Rtww-offline/build/x64/Release/Triangles");
	if (!fs::exists(imageParentPath)) {
		fs::create_directory(imageParentPath);
	}
	FrameRenderer renderer("Triangles", imageParentPath, 20, 8);
	
	Color background(0, 0, 0);
	Vector3f vup(0, 1, 0);
	Point3f lookfrom = Point3f(0, 550, -1800);
	Point3f lookat = Point3f(0, 100, 400);
	auto vfov = 40.0;
	auto dist2Focus = 10.0f;
	auto aperture = 0.0;
	
	auto settings = std::make_shared<FrameSettings>();
	settings->SetImageOptions(400, 400);
	settings->SetRayTraceOptions(20, 1000);
	settings->SetScene(std::make_shared<Camera>(lookfrom, lookat, vup, vfov, 1.0f, aperture, dist2Focus),
		std::make_shared<ShapesSet>(CornellBoxModel()), lights, background);
	renderer.AddFrame(settings);
	
	renderer.Render(Draw, 0, 1);

	return 0;
}