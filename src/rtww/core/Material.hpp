#pragma
#include "Core.hpp"
#include "Texture.hpp"

struct IntersectionRecord;

class Material {
public:
	virtual bool Scatter(const Ray& r, const IntersectionRecord& rec, Color& attenuation, Ray& scattered) const = 0;
};

class Lambertian :public Material {
public:
	Lambertian(const Color& c) :albedo(std::make_shared<SolidColorTexture>(c)) {}
	Lambertian(std::shared_ptr<Texture> a) :albedo(a) {}

	virtual bool Scatter(const Ray& r, const IntersectionRecord& rec, Color& attenuation, Ray& scattered) const override {
		auto scatterDir = rec.normal + RandomUnitVec();

		if (scatterDir.IsNearZero())
			scatterDir = rec.normal;

		scattered = Ray(rec.hitPoint, scatterDir);
		attenuation = albedo->Value(rec.u, rec.v, rec.hitPoint);
		return true;
	}

public:
	std::shared_ptr<Texture> albedo;
};

class Metal : public Material {
public:
	Metal(const Color& c, Float f) :albedo(c), fuzz(f < 1.0f ? f : 1.0f) {}

	virtual bool Scatter(const Ray& r, const IntersectionRecord& rec, Color& attenuation, Ray& scattered) const override {
		Vector3f reflected = r.direction.Normalize().Reflect(rec.normal);
		scattered = Ray(rec.hitPoint, reflected + fuzz * RandomInUnitSphere());
		attenuation = albedo;
		return Dot(scattered.direction, rec.normal) > 0;
	}

public:
	Color albedo;
	Float fuzz;
};

class Dielectric : public Material {
public:
	Dielectric(/*const Color& c, */Float ir) :/*albedo(c), */indexOfRefraction(ir) {}

	virtual bool Scatter(const Ray& r, const IntersectionRecord& rec, Color& attenuation, Ray& scattered) const override {
		attenuation = Color(1.0, 1.0, 1.0);//TODO albedo * 0.3f;
		Float refractionRatio = rec.isFrontFace ? (1.0f / indexOfRefraction) : indexOfRefraction;
		auto unitDir = r.direction.Normalize();

		Float cosTheta = fmin(Dot(-unitDir, rec.normal), 1.0f);
		Float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
		bool canRefraction = refractionRatio * sinTheta <= 1.0f;
		Vector3f dir;
		if(!canRefraction || Reflectance(cosTheta, refractionRatio) > Random<Float>()) 
			dir = unitDir.Reflect(rec.normal);
		else 
			dir = unitDir.Refract(rec.normal, refractionRatio);

		scattered = Ray(rec.hitPoint, dir);
		return true;
	}

private:
	static double Reflectance(double cosine, double refractionRatio) {
		//Schlick's approximation
		auto r0 = (1 - refractionRatio) / (1 + refractionRatio);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}

public:
	//Color albedo;
	Float indexOfRefraction;
};