#include "../core/Core.hpp"
#include "../shape/Sphere.hpp"

const uint16_t imageWidth = 1024;
const double aspectRatio = 16.0f / 9.0f;
const uint16_t imageHeight = static_cast<int>(imageWidth / aspectRatio);

Color RayColor(const Ray& r){
	Sphere* s = new Sphere(Point3f(0, 0, -1), 0.5f);
	IntersectionRecord rec;
	if (!s->Intersection(r, -1000, 1000, rec))
		return Color(0, 0, 0);
	auto t = rec.time;
	if (t > 0.0){
		Vector3f N = (r.At(t) - Point3f(0, 0, -1)).Normalize();
		return 0.5f * Color(N.x + 1, N.y + 1, N.z + 1);
	}
	Vector3f normalizedDirection = r.direction.Normalize();
	t = 0.5f*(normalizedDirection.y + 1.0f);
	return (1.0f - t)*Color(1.0f, 1.0f, 1.0f) + t * Color(0.5f, 0.7f, 1.0f);
}

int main(int argc, char** argv) {
	double viewportHeight = 2.0f;
	double viewportWidth = aspectRatio * viewportHeight;
	double focalLength = 1.0f;
	
	Point3f origin = Point3f(0, 0, 0);
	Vector3f horizontal = Vector3f(viewportWidth, 0, 0);
	Vector3f vectical = Vector3f(0, viewportHeight, 0);
	auto lowerLeftCorner = origin - horizontal / 2 - vectical / 2 - Vector3f(0, 0, focalLength);

	std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";
	for (int j = imageHeight - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << (double(j) / imageHeight) * 100.0f << std::flush;
		for (int i = 0; i < imageWidth; ++i) {
			auto u = double(i) / (imageWidth - 1);
			auto v = double(j) / (imageHeight - 1);
			Ray r(origin, lowerLeftCorner + u * horizontal + v * vectical - origin);
			std::cout << RayColor(r);
		}
	}
	std::cerr << "\nDone.\n";

	return 0;
}