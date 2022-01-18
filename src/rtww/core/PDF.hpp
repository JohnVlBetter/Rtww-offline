#pragma once

#include "Shape.hpp"
#include "OrthonormalBasis.hpp"

class PDF {
public:
	virtual ~PDF() {}

	virtual double Value(const Vector3f& direction) const = 0;
	virtual Vector3f Generate() const = 0;
};


class CosinePDF : public PDF {
public:
	CosinePDF(const Vector3f& w) { uvw.BuildFromW(w); }

	virtual double Value(const Vector3f& direction) const override {
		auto cosine = Dot(direction.Normalize(), uvw.w());
		return (cosine <= 0) ? 0 : cosine / Pi;
	}

	virtual Vector3f Generate() const override {
		return uvw.Local(RandomCosineDirection());
	}

public:
	OrthonormalBasis uvw;
};


class ShapePDF : public PDF {
public:
	ShapePDF(std::shared_ptr<Shape> p, const Point3f& origin) : ptr(p), o(origin) {}

	virtual double Value(const Vector3f& direction) const override {
		return ptr->PDFValue(o, direction);
	}

	virtual Vector3f Generate() const override {
		return ptr->ShapeRandom(o);
	}

public:
	Point3f o;
	std::shared_ptr<Shape> ptr;
};


class MixturePDF : public PDF {
public:
	MixturePDF(std::shared_ptr<PDF> p0, std::shared_ptr<PDF> p1) {
		p[0] = p0;
		p[1] = p1;
	}

	virtual double Value(const Vector3f& direction) const override {
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