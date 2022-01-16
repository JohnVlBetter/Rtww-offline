#pragma once

#include "Core.hpp"
#include "OrthonormalBasis.hpp"

class PDF {
public:
	virtual ~PDF() {}
	virtual Float Value(const Vector3f& dir) const = 0;
	virtual Vector3f Generate() const = 0;
};

class CosinePDF : public PDF {
public:
	CosinePDF(const Vector3f& dir) { onb = std::make_shared<OrthonormalBasis>(dir); }

	virtual Float Value(const Vector3f& dir) const {
		auto cosine = Dot(dir.Normalize(), onb->zAxis);
		return (cosine <= 0) ? 0 : cosine / Pi;
	}
	virtual Vector3f Generate() const {
		return onb->Local(RandomCosineDirection());
	}
public:
	std::shared_ptr<OrthonormalBasis> onb;
};

class ShapeDirPDF : public PDF {
public:
	ShapeDirPDF(std::shared_ptr<Shape> ptr, const Point3f& o) : shapePtr(ptr), origin(o) {}

	virtual Float Value(const Vector3f& dir) const override {
		return shapePtr->PDFValue(origin, dir);
	}

	virtual Vector3f Generate() const override {
		return shapePtr->Random(origin);
	}

public:
	Point3f origin;
	std::shared_ptr<Shape> shapePtr;
};

class MixturePDF : public PDF {
public:
	MixturePDF(std::shared_ptr<PDF> p0, std::shared_ptr<PDF> p1) {
		p[0] = p0;
		p[1] = p1;
	}

	virtual Float Value(const Vector3f& direction) const override {
		return 0.5 * p[0]->Value(direction) + 0.5 *p[1]->Value(direction);
	}

	virtual Vector3f Generate() const override {
		if (Random<Float>() < 0.5)
			return p[0]->Generate();
		else
			return p[1]->Generate();
	}

public:
	std::shared_ptr<PDF> p[2];
};