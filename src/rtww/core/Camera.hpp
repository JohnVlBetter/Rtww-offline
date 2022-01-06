#pragma once

#include "Core.hpp"

class Camera {
public:
	Camera(){
		double aspectRatio = 16.0f / 9.0f;
		double viewportHeight = 2.0f;
		double viewportWidth = aspectRatio * viewportHeight;
		double focalLength = 1.0f;

		origin = Point3f(0, 0, 0);
		horizontal = Vector3f(viewportWidth, 0, 0);
		vectical = Vector3f(0, viewportHeight, 0);
		lowerLeftCorner = origin - horizontal / 2 - vectical / 2 - Vector3f(0, 0, focalLength);
	}

	Ray GenerateRay(Float u, Float v) const {
		return Ray(origin, lowerLeftCorner + u * horizontal + v * vectical - origin);
	}

public:
	Point3f origin;
	Vector3f horizontal;
	Vector3f vectical;
	Point3f lowerLeftCorner;
};