#pragma once

#include "hittable.hpp"

#include <memory>
#include <vector>

class HittableList : public Hittable {
public:
	HittableList() {}
	HittableList(std::shared_ptr<Hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(std::shared_ptr<Hittable> object) { objects.push_back(object); }

	virtual bool hit(const Ray &ray, double t_min, double t_max, HitRecord &record) const override;

private:
	std::vector<std::shared_ptr<Hittable>> objects;
};