#pragma
#include "Core.hpp"

struct IntersectionRecord;

class Material {
public:
	virtual bool Scatter(const Ray& r, const IntersectionRecord& rec, Color& attenuation, Ray& scattered) const = 0;
};

class Lambertian :public Material {
public:
	Lambertian(const Color& c) :albedo(c) {}

	virtual bool Scatter(const Ray& r, const IntersectionRecord& rec, Color& attenuation, Ray& scattered) const override {
		auto scatterDir = rec.normal + RandomUnitVec();

		if (scatterDir.IsNearZero())
			scatterDir = rec.normal;

		scattered = Ray(rec.hitPoint, scatterDir);
		attenuation = albedo;
		return true;
	}

public:
	Color albedo;
};

class Metal : public Material {
public:
	Metal(const Color& c) :albedo(c) {}

	virtual bool Scatter(const Ray& r, const IntersectionRecord& rec, Color& attenuation, Ray& scattered) const override {
		Vector3f reflected = r.direction.Normalize().Reflect(rec.normal);
		scattered = Ray(rec.hitPoint, reflected);
		attenuation = albedo;
		return Dot(scattered.direction, rec.normal) > 0;
	}

public:
	Color albedo;
};