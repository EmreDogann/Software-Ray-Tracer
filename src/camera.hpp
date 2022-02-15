#pragma once

#include "math_utils.hpp"
#include "ray.hpp"
#include "vector3.hpp"

class Camera {
public:
	Camera(Vec3 lookFrom, Vec3 lookAt, Vec3 vUp, double verticalFOV, double aspectRatio, double aperture, double focusDistance) {
		double viewportHeight = 2.0 * tan(degrees_to_radians(verticalFOV) / 2);
		double viewportWidth = aspectRatio * viewportHeight;

		front = unit_vector(lookFrom - lookAt);
		left = unit_vector(cross(vUp, front));
		up = cross(front, left);

		origin = lookFrom;
		horizontal = focusDistance * viewportWidth * left;
		vertical = focusDistance * viewportHeight * up;
		lowerLeftCorner = origin - horizontal / 2 - vertical / 2 - focusDistance * front;
		lensRadius = aperture / 2;
	}

	Ray getRay(double s, double t) const {
		Vec3 rd = lensRadius * random_in_unit_disk();
		Vec3 offset = left * rd.x() + up * rd.y();

		return Ray(origin + offset, lowerLeftCorner + s * horizontal + t * vertical - origin - offset);
	}

private:
	Vec3 origin;
	Vec3 lowerLeftCorner;
	Vec3 horizontal;
	Vec3 vertical;
	Vec3 left, up, front;
	double lensRadius;
};