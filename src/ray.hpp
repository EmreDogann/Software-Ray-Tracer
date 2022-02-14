#pragma once

#include "vector3.hpp"

class Ray {
public:
	Ray() {}
	Ray(const Vec3 &origin, const Vec3 &direction) : originVec{origin}, directionVec(direction) {}

	Vec3 origin() const { return originVec; }
	Vec3 direction() const { return directionVec; }

	Vec3 at(double t) const { return originVec + t * directionVec; }

private:
	Vec3 originVec;
	Vec3 directionVec;
};