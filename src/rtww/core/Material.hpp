#pragma
#include "Core.hpp"
#include "Texture.hpp"
#include "PDF.hpp"

struct IntersectionRecord;

struct ScatterRecord {
	Ray specularRay;
	bool isSpecular;
	Color attenuation;
	std::shared_ptr<PDF> pdfPtr;
};

class Material {
public:
	virtual bool Scatter(const Ray& r, const IntersectionRecord& rec, ScatterRecord& srec) const { return false; };
	virtual Float ScatteringPDF(const Ray& r, const IntersectionRecord& rec, const Ray& scattered) const { return 0; }
	virtual Color Emitted(const Ray& r, const IntersectionRecord& rec, Float u, Float v, const Point3f& p) const { return Color(0, 0, 0); }
};

class Lambertian :public Material {
public:
	Lambertian(const Color& c) :albedo(std::make_shared<SolidColorTexture>(c)) {}
	Lambertian(std::shared_ptr<Texture> a) :albedo(a) {}

	virtual bool Scatter(const Ray& r, const IntersectionRecord& rec, ScatterRecord& srec) const override {
		srec.isSpecular = false;
		srec.attenuation = albedo->Value(rec.u, rec.v, rec.hitPoint);
		srec.pdfPtr = std::make_shared<CosinePDF>(rec.normal);
		return true;
	}
	virtual Float ScatteringPDF(const Ray& r, const IntersectionRecord& rec, const Ray& scattered) const {
		auto cosine = Dot(rec.normal, scattered.direction.Normalize());
		return cosine < 0 ? 0 : cosine / Pi;
	}


public:
	std::shared_ptr<Texture> albedo;
};

class Metal : public Material {
public:
	Metal(const Color& c, Float f) :albedo(c), fuzz(f < 1.0f ? f : 1.0f) {}

	virtual bool Scatter(const Ray& r, const IntersectionRecord& rec, ScatterRecord& srec) const override {
		Vector3f reflected = r.direction.Normalize().Reflect(rec.normal);
		srec.specularRay = Ray(rec.hitPoint, reflected + fuzz * RandomInUnitSphere());
		srec.attenuation = albedo;
		srec.isSpecular = true;
		srec.pdfPtr = 0;
		return true;
	}

public:
	Color albedo;
	Float fuzz;
};

class Dielectric : public Material {
public:
	Dielectric(/*const Color& c, */Float ir) :/*albedo(c), */indexOfRefraction(ir) {}

	virtual bool Scatter(const Ray& r, const IntersectionRecord& rec, ScatterRecord& srec) const override {
		srec.isSpecular = true;
		srec.pdfPtr = nullptr;
		srec.attenuation = Color(1.0, 1.0, 1.0);
		Float refractionRatio = rec.isFrontFace ? (1.0f / indexOfRefraction) : indexOfRefraction;
		auto unitDir = r.direction.Normalize();

		Float cosTheta = fmin(Dot(-unitDir, rec.normal), 1.0f);
		Float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
		bool canRefraction = refractionRatio * sinTheta <= 1.0f;
		Vector3f dir;
		if (!canRefraction || Reflectance(cosTheta, refractionRatio) > Random<Float>())
			dir = unitDir.Reflect(rec.normal);
		else
			dir = unitDir.Refract(rec.normal, refractionRatio);

		srec.specularRay = Ray(rec.hitPoint, dir, r.time);
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

class DiffuseLight : public Material {
public:
	DiffuseLight(std::shared_ptr<Texture> a) : emit(a) {}
	DiffuseLight(Color c) : emit(std::make_shared<SolidColorTexture>(c)) {}

	virtual bool Scatter(const Ray& r, const IntersectionRecord& rec, ScatterRecord& srec) const override {
		return false;
	}

	virtual Color Emitted(const Ray& r, const IntersectionRecord& rec, Float u, Float v, const Point3f& p) const override {
		if (!rec.isFrontFace)
			return Color(0, 0, 0);
		return emit->Value(u, v, p);
	}

public:
	std::shared_ptr<Texture> emit;
};

//class Isotropic : public Material {
//public:
//	Isotropic(Color c) : albedo(std::make_shared<SolidColorTexture>(c)) {}
//	Isotropic(std::shared_ptr<Texture> a) : albedo(a) {}
//
//	virtual bool Scatter(const Ray& r, const IntersectionRecord& rec, Color& attenuation, Ray& scattered, Float& pdf) const override {
//		scattered = Ray(rec.hitPoint, RandomInUnitSphere(), r.time);
//		attenuation = albedo->Value(rec.u, rec.v, rec.hitPoint);
//		return true;
//	}
//
//public:
//	std::shared_ptr<Texture> albedo;
//};