#pragma once

#include "hittable.hpp"
#include "vector3.hpp"

class Sphere : public Hittable {
public:
	Sphere(){};
	Sphere(Vec3 center, double radius) : center{center}, radius{radius} {};

	virtual bool hit(const Ray &ray, double t_min, double t_max, HitRecord &record) const override;

private:
	Vec3 center;
	double radius;
};