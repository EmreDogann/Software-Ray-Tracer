#pragma once

#include "ray.hpp"

struct HitRecord {
	Vec3 position;
	Vec3 normal;
	double t;
	bool frontFace;

	// Will compare the two vectors and ensure that the surface normal is always facing against the ray's direction.
	inline void setFaceNormal(const Ray &ray, const Vec3 &outwardNormal) {
		frontFace = dot(ray.direction(), outwardNormal) < 0;
		normal = frontFace ? outwardNormal : -outwardNormal;
	}
};

class Hittable {
public:
	virtual bool hit(const Ray &ray, double t_min, double t_max, HitRecord &record) const = 0;
};