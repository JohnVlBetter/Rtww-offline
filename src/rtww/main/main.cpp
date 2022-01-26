#include "core/Core.hpp"
#include "shape/Sphere.hpp"
#include "shape/Medium.hpp"
#include "shape/Box.hpp"
#include "shape/Rectangle.hpp"
#include "shape/FlipFace.hpp"
#include "core/World.hpp"
#include "core/Camera.hpp"
//#include "core/Material.hpp"
#include "core/BVH.hpp"
#include "core/Texture.hpp"
#include "core/PDF.hpp"
#include "core/ThreadPool.h"
#include "core/Frame.hpp"
#include <thread>
#include <Windows.h>

const uint16_t imageWidth = 400;
const double aspectRatio = 1.0f;
const int depth = 50;
const uint16_t imageHeight = static_cast<int>(imageWidth / aspectRatio);

Color RayColor(const Ray& r, const Color& background, std::shared_ptr <ShapesSet> world, std::shared_ptr<Shape> lights, int depth) {
	IntersectionRecord rec;

	if (depth <= 0)
		return Color(0, 0, 0);

	if (!world->Intersection(r, 0.001f, Infinity, rec)) {
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

/*ShapesSet Earth() {
	auto earthTexture = std::make_shared<ImageTexture>("../../../resources/earthmap.jpg");
	auto earthSurface = std::make_shared<Lambertian>(earthTexture);
	auto globe = std::make_shared<Sphere>(Point3f(0, 0, 0), 2, earthSurface);

	return ShapesSet(globe);
}

ShapesSet RandomScene() {
	ShapesSet world;

	auto checker = std::make_shared<CheckerTexture>(Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
	world.Add(std::make_shared<Sphere>(Point3f(0, -1000, 0), 1000, std::make_shared<Lambertian>(checker)));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = Random<Float>();
			Point3f center(a + 0.9 * Random<Float>(), 0.2, b + 0.9 * Random<Float>());

			if ((center - Point3f(4, 0.2, 0)).Length() > 0.9) {
				std::shared_ptr<Material> SphereMaterial;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = RandomVec();// *Color::Random<Float>();
					SphereMaterial = std::make_shared<Lambertian>(albedo);
					world.Add(std::make_shared<Sphere>(center, 0.2, SphereMaterial));
				}
				else if (choose_mat < 0.95) {
					// Metal
					auto albedo = RandomVec(0.5, 1);
					auto fuzz = Random<Float>(0, 0.5);
					SphereMaterial = std::make_shared<Metal>(albedo, fuzz);
					world.Add(std::make_shared<Sphere>(center, 0.2, SphereMaterial));
				}
				else {
					// glass
					SphereMaterial = std::make_shared<Dielectric>(1.5);
					world.Add(std::make_shared<Sphere>(center, 0.2, SphereMaterial));
				}
			}
		}
	}

	auto material1 = std::make_shared<Dielectric>(1.5);
	world.Add(std::make_shared<Sphere>(Point3f(0, 1, 0), 1.0, material1));

	auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
	world.Add(std::make_shared<Sphere>(Point3f(-4, 1, 0), 1.0, material2));

	auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
	world.Add(std::make_shared<Sphere>(Point3f(4, 1, 0), 1.0, material3));

	return ShapesSet(std::make_shared<BVHNode>(world, 0.0, 1.0));
}

ShapesSet SimpleLight() {
	ShapesSet objects;

	auto tex = std::make_shared<SolidColorTexture>(Color(0.8,0.75,1.0));
	objects.Add(std::make_shared<Sphere>(Point3f(0, -1000, 0), 1000, std::make_shared<Lambertian>(tex)));
	objects.Add(std::make_shared<Sphere>(Point3f(0, 2, 0), 2, std::make_shared<Lambertian>(tex)));

	auto difflight1 = std::make_shared<DiffuseLight>(Color(1.5, 1.5, 2));
	auto difflight2 = std::make_shared<DiffuseLight>(Color(3, 2, 2));
	objects.Add(std::make_shared<RectangleXY>(3, 5, 1, 3, -2, difflight1));
	objects.Add(std::make_shared<Sphere>(Point3f(0, 6.5, 0), 2, difflight2));

	return objects;
}
*/
ShapesSet CornellBox() {
	ShapesSet objects;

	auto red = std::make_shared<Lambertian>(Color(.65, .05, .05));
	auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
	auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
	auto light = std::make_shared<DiffuseLight>(Color(15, 15, 15));

	objects.Add(std::make_shared<RectangleYZ>(0, 555, 0, 555, 555, green));
	objects.Add(std::make_shared<RectangleYZ>(0, 555, 0, 555, 0, red));
	objects.Add(std::make_shared<FlipFace>(std::make_shared<RectangleXZ>(213, 343, 227, 332, 554, light)));
	objects.Add(std::make_shared<RectangleXZ>(0, 555, 0, 555, 0, white));
	objects.Add(std::make_shared<RectangleXZ>(0, 555, 0, 555, 555, white));
	objects.Add(std::make_shared<RectangleXY>(0, 555, 0, 555, 555, white));

	std::shared_ptr<Material> aluminum = std::make_shared<Metal>(Color(0.8, 0.85, 0.88), 0.0);
	std::shared_ptr<Shape> box1 = std::make_shared<Box>(Point3f(0, 0, 0), Point3f(165, 330, 165), aluminum);
	box1 = std::make_shared<TRotateY>(box1, 15);
	box1 = std::make_shared<TTranslate>(box1, Vector3f(265, 0, 295));
	objects.Add(box1);
	//objects.Add(std::make_shared<Box>(Point3f(265, 0, 295), Point3f(430, 330, 460), white));

	auto glass = std::make_shared<Dielectric>(1.5);
	objects.Add(std::make_shared<Sphere>(Point3f(190, 90, 190), 90, glass));

	return objects;
}

/*ShapesSet CornellSmoke() {
	ShapesSet objects;

	auto red = std::make_shared<Lambertian>(Color(.65, .05, .05));
	auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
	auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
	auto light = std::make_shared<DiffuseLight>(Color(7, 7, 7));

	objects.Add(std::make_shared<RectangleYZ>(0, 555, 0, 555, 555, green));
	objects.Add(std::make_shared<RectangleYZ>(0, 555, 0, 555, 0, red));
	objects.Add(std::make_shared<RectangleXZ>(113, 443, 127, 432, 554, light));
	objects.Add(std::make_shared<RectangleXZ>(0, 555, 0, 555, 555, white));
	objects.Add(std::make_shared<RectangleXZ>(0, 555, 0, 555, 0, white));
	objects.Add(std::make_shared<RectangleXY>(0, 555, 0, 555, 555, white));

	std::shared_ptr<Shape> box1 = std::make_shared<Box>(Point3f(0, 0, 0), Point3f(165, 330, 165), white);

	std::shared_ptr<Shape> box2 = std::make_shared<Box>(Point3f(0, 0, 0), Point3f(165, 165, 165), white);

	objects.Add(std::make_shared<Medium>(box1, 0.01, Color(0, 0, 0)));
	objects.Add(std::make_shared<Medium>(box2, 0.01, Color(1, 1, 1)));

	return objects;
}

ShapesSet FinalScene() {
	ShapesSet boxes1;
	auto ground = std::make_shared<Lambertian>(Color(0.48, 0.83, 0.53));

	const int boxes_per_side = 20;
	for (int i = 0; i < boxes_per_side; i++) {
		for (int j = 0; j < boxes_per_side; j++) {
			auto w = 100.0;
			auto x0 = -1000.0 + i * w;
			auto z0 = -1000.0 + j * w;
			auto y0 = 0.0;
			auto x1 = x0 + w;
			auto y1 = Random<Float>(1, 101);
			auto z1 = z0 + w;

			boxes1.Add(std::make_shared<Box>(Point3f(x0, y0, z0), Point3f(x1, y1, z1), ground));
		}
	}

	ShapesSet objects;

	objects.Add(std::make_shared<BVHNode>(boxes1, 0, 1));

	auto light = std::make_shared<DiffuseLight>(Color(7, 7, 7));
	objects.Add(std::make_shared<RectangleXZ>(123, 423, 147, 412, 554, light));

	auto center1 = Point3f(400, 400, 200);
	auto sphere_material = std::make_shared<Lambertian>(Color(0.7, 0.3, 0.1));
	objects.Add(std::make_shared<Sphere>(center1, 50, sphere_material));

	objects.Add(std::make_shared<Sphere>(Point3f(260, 150, 45), 50, std::make_shared<Dielectric>(1.5)));
	objects.Add(std::make_shared<Sphere>(
		Point3f(0, 150, 145), 50, std::make_shared<Metal>(Color(0.8, 0.8, 0.9), 1.0)
		));

	auto boundary = std::make_shared<Sphere>(Point3f(360, 150, 145), 70, std::make_shared<Dielectric>(1.5));
	objects.Add(boundary);
	objects.Add(std::make_shared<Medium>(boundary, 0.2, Color(0.2, 0.4, 0.9)));
	boundary = std::make_shared<Sphere>(Point3f(0, 0, 0), 5000, std::make_shared<Dielectric>(1.5));
	objects.Add(std::make_shared<Medium>(boundary, .0001, Color(1, 1, 1)));

	auto emat = std::make_shared<Lambertian>(std::make_shared<ImageTexture>("../../../resources/earthmap.jpg"));
	objects.Add(std::make_shared<Sphere>(Point3f(400, 200, 400), 100, emat));
	objects.Add(std::make_shared<Sphere>(Point3f(220, 280, 300), 80, std::make_shared<Metal>(Color(0.9, 0.9, 0.9), 0.9)));

	ShapesSet boxes2;
	auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
	int ns = 1000;
	for (int j = 0; j < ns; j++) {
		boxes2.Add(std::make_shared<Sphere>(RandomPoint(0, 165) + Point3f(-100, 270, 395), 10, white));
	}
	objects.Add(std::make_shared<BVHNode>(boxes2, 0.0, 1.0));

	return objects;
}*/

ShapesSet CornellBox2() {
	ShapesSet objects;

	auto red = std::make_shared<Lambertian>(Color(.65, .05, .05));
	auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
	auto pink = std::make_shared<Lambertian>(Color(1, .412, .706));
	auto blue = std::make_shared<Lambertian>(Color(.12, .15, .65));
	auto light = std::make_shared<DiffuseLight>(Color(15, 15, 15));

	objects.Add(std::make_shared<RectangleYZ>(0, 555, 0, 555, 605, blue));
	objects.Add(std::make_shared<RectangleYZ>(0, 555, 0, 555, -50, red));
	objects.Add(std::make_shared<FlipFace>(std::make_shared<RectangleXZ>(200, 356, 214, 345, 554, light)));
	objects.Add(std::make_shared<RectangleXZ>(-50, 605, -200, 555, 0, white));
	objects.Add(std::make_shared<RectangleXZ>(-50, 605, 0, 555, 555, white));
	objects.Add(std::make_shared<RectangleXY>(-50, 605, 0, 555, 555, white));

	std::shared_ptr<Material> aluminum = std::make_shared<Metal>(Color(0.8, 0.85, 0.88), 0.0);
	std::shared_ptr<Shape> box1 = std::make_shared<Box>(Point3f(0, 0, 0), Point3f(165, 350, 165), aluminum);
	box1 = std::make_shared<TRotateY>(box1, 40);
	box1 = std::make_shared<TTranslate>(box1, Vector3f(0, 0, 275));
	objects.Add(box1);

	std::shared_ptr<Shape> box2 = std::make_shared<Box>(Point3f(0, 0, 0), Point3f(165, 280, 165), pink);
	box2 = std::make_shared<TRotateY>(box2, 50);
	box2 = std::make_shared<TTranslate>(box2, Vector3f(350, 1.5, 275));
	objects.Add(box2);
	//objects.Add(std::make_shared<Box>(Point3f(265, 0, 295), Point3f(430, 330, 460), white));

	auto glass = std::make_shared<Dielectric>(1.5);
	objects.Add(std::make_shared<Sphere>(Point3f(275, 75, 190), 75, glass));

	return objects;
}

std::vector<std::vector<Color>> pixels(imageWidth, std::vector<Color>(imageHeight, Color(0, 0, 0)));


Color background(0.00, 0.00, 0.00);
Vector3f vup(0, 1, 0);
Point3f lookfrom = Point3f(278, 278, -800);
Point3f lookat = Point3f(278, 278, 0);
auto vfov = 40.0;
auto dist2Focus = 10.0f;
auto aperture = 0.0;

std::vector<Color> Draw(int index, std::shared_ptr<FrameSettings> settings) {
	std::vector<Color> t(settings->imageHeight, Color(0, 0, 0));
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
	lights->Add(std::make_shared<RectangleXZ>(200, 356, 214, 345, 554, std::shared_ptr<Material>()));
	lights->Add(std::make_shared<Sphere>(Point3f(275, 75, 190), 75, std::shared_ptr<Material>()));
	
	fs::path imageParentPath("D:/Workspace/CG/Repos/Rtww-offline/build/x64/Release/test");
	if(!fs::exists(imageParentPath)){
		fs::create_directory(imageParentPath);
	}
	FrameRenderer renderer(imageParentPath, 8);


	for (int i = 0; i < 30; ++i){
		auto settings = std::make_shared<FrameSettings>();
		settings->SetImageOptions(imageWidth, imageHeight);
		settings->SetRayTraceOptions(50, 100);
		settings->SetScene(std::make_shared<Camera>(lookfrom + Vector3f(100 + (-10 * i), -70 + (4.3 * i), 26.8 * i), 
			lookat + Vector3f(0, -6.7 * i, 10 * i), vup, vfov, aspectRatio, aperture, dist2Focus),
			std::make_shared<ShapesSet>(CornellBox()), lights, Color(0, 0, 0));
		renderer.AddFrame(settings);
	}

	renderer.Render(Draw, 0, 30);
	return 0;
}