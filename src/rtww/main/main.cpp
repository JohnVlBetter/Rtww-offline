#include "../core/Core.hpp"
#include "../shape/Sphere.hpp"
#include "../core/World.hpp"
#include "../core/Camera.hpp"
#include "../core/Material.hpp"
#include "../core/BVH.hpp"
#include "../core/Texture.hpp"

const uint16_t imageWidth = 200;
const double aspectRatio = 3.0f / 2.0f;
const int depth = 40;
const uint16_t imageHeight = static_cast<int>(imageWidth / aspectRatio);

Color RayColor(const Ray& r, const ShapesSet& world, int depth){
	IntersectionRecord rec;

	if (depth <= 0)
		return Color(0, 0, 0);

	if (world.Intersection(r, 0.001f, Infinity, rec)){
		Ray scattered;
		Color attenuation;
		if (rec.matPtr->Scatter(r, rec, attenuation, scattered))
			return attenuation * RayColor(scattered, world, depth - 1);
		return Color(0, 0, 0);
	}
	Vector3f normalizedDirection = r.direction.Normalize();
	auto t = 0.5f*(normalizedDirection.y + 1.0f);
	return (1.0f - t)*Color(1.0f, 1.0f, 1.0f) + t * Color(0.5f, 0.7f, 1.0f);
}

ShapesSet Earth() {
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

int main(int argc, char** argv) {
	/*Point3f lookfrom(13, 2, 3);
	Point3f lookat(0, 0, 0);*/
	Vector3f vup(0, 1, 0);
	Point3f lookfrom = Point3f(13, 2, 3);
	Point3f lookat = Point3f(0, 0, 0);
	auto vfov = 20.0;
	auto dist2Focus = 10.0f;
	auto aperture = 0.0;
	Camera camera(lookfrom, lookat, vup, vfov, aspectRatio, aperture, dist2Focus);

	ShapesSet world = Earth();
	
	std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";
	for (int j = imageHeight - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << (double(j) / imageHeight) * 100.0f << std::flush;
		for (int i = 0; i < imageWidth; ++i) {
			Color pixelColor(0, 0, 0);
			for (int k = 0; k < samplesPerPixel; ++k){
				auto u = Float(i + Random<Float>()) / (imageWidth - 1);
				auto v = Float(j + Random<Float>()) / (imageHeight - 1);
				Ray r = camera.GenerateRay(u, v);
				pixelColor += RayColor(r, world, depth);
			}
			std::cout << pixelColor;
		}
	}
	std::cerr << "\nDone.\n";

	return 0;
}