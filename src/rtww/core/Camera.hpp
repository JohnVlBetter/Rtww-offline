#pragma once

#include "Core.hpp"

class Camera {
public:
	Camera(Point3f lookFrom, Point3f lookAt, Vector3f vUp, Float vFov/*vertical field of view in degrees*/, 
		Float aspectRatio, Float aperture, Float focusDistance){
		auto theta = Radians(vFov);
		auto h = tan(theta / 2);
		auto viewportHeight = 2.0 * h;
		double viewportWidth = aspectRatio * viewportHeight;

		dir = (lookFrom - lookAt).Normalize();
		right = Cross(vUp, dir).Normalize();
		up = Cross(dir, right);

		origin = lookFrom;
		horizontal = focusDistance * viewportWidth * right;
		vectical = focusDistance * viewportHeight * up;
		lowerLeftCorner = lookFrom - horizontal / 2 - vectical / 2 - focusDistance * dir;

		lenRadius = aperture / 2;
	}

	Ray GenerateRay(Float u, Float v) const {
		Vector2f randomDisk = lenRadius * RandomInUnitDisk();
		Vector3f offset = right * randomDisk.x + up * randomDisk.y;

		return Ray(origin + offset, lowerLeftCorner + u * horizontal + v * vectical - origin - offset);
	}

public:
	Point3f origin;
	Vector3f horizontal;
	Vector3f vectical;
	Vector3f dir;
	Vector3f right;
	Vector3f up;
	Float lenRadius;
	Point3f lowerLeftCorner;
};