#include "../core/Core.hpp"
#include "../shape/Sphere.hpp"
#include "../core/World.hpp"
#include "../core/Camera.hpp"

const uint16_t imageWidth = 400;
const double aspectRatio = 16.0f / 9.0f;
const int depth = 50;
const uint16_t imageHeight = static_cast<int>(imageWidth / aspectRatio);

Color RayColor(const Ray& r, const ShapesSet& world, int depth){
	IntersectionRecord rec;

	if (depth <= 0)
		return Color(0, 0, 0);

	if (world.Intersection(r, 0.001f, Infinity, rec)){
		Point3f target = rec.hitPoint + RandomInHemisphere(rec.normal);
		return 0.5 * RayColor(Ray(rec.hitPoint, target - rec.hitPoint), world, depth-1);
	}
	Vector3f normalizedDirection = r.direction.Normalize();
	auto t = 0.5f*(normalizedDirection.y + 1.0f);
	return (1.0f - t)*Color(1.0f, 1.0f, 1.0f) + t * Color(0.5f, 0.7f, 1.0f);
}

int main(int argc, char** argv) {	
	Camera camera;

	ShapesSet world;
	world.Add(std::make_shared<Sphere>(Point3f(0, 0, -1), 0.5f));
	world.Add(std::make_shared<Sphere>(Point3f(0, -100.5f, -1), 100));
	
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