#pragma once

#include "hittable.hpp"
#include <memory>

class Sphere : public Hittable {
public:
	Sphere(){};
	Sphere(Vec3 center, double radius, std::shared_ptr<Material> material) : center{center}, radius{radius}, material{material} {};

	virtual bool hit(const Ray &ray, double t_min, double t_max, HitRecord &record) const override;

private:
	Vec3 center;
	double radius;
	std::shared_ptr<Material> material{};
};