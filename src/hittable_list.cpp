#include "hittable_list.hpp"

bool HittableList::hit(const Ray &ray, double t_min, double t_max, HitRecord &record) const {
	HitRecord tempRecord;
	bool hitAnything = false;
	double closestSoFar = t_max;

	for (const std::shared_ptr<Hittable> &object : objects) {
		if (object->hit(ray, t_min, closestSoFar, tempRecord)) {
			hitAnything = true;
			closestSoFar = tempRecord.t;
			record = tempRecord;
		}
	}

	return hitAnything;
}