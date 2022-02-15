#include "sphere.hpp"

/* Solves the quadratic equation to solve for t.
    x = t
    a = D^2
    NOTE: If the ray's direction vector is normalized, then a = 1.
    This is because a normalized vector doted with itself is length^2, so no need to perform an unnecessary calculation.

    b = D(O - C)
    c = |O - C|^2 - R^2

    O = Ray's origin
    C = Sphere's center
    D = Ray's Direction
    R = Sphere's radius
*/
bool Sphere::hit(const Ray &ray, double t_min, double t_max, HitRecord &record) const {
	Vec3 oc = ray.origin() - center;
	double a = ray.direction().length_squared();
	double halfB = dot(ray.direction(), oc);
	double c = oc.length_squared() - radius * radius;

	/*
	    Discriminant will tell us the number of roots (solutions) to the intersection
	    Discriminant < 0 : The ray did not intersect with the sphere.
	    Discriminant = 0 : The ray has one intersection point with the sphere (is at tangent to the sphere).
	    Discriminant > 0 : The ray has two intersection points with the sphere (the ray has shot through the sphere - entry and exit point).
	*/
	double discriminant = (halfB * halfB) - (a * c);
	if (discriminant < 0) {
		return false; // Ray did not interect with the sphere.
	}

	// Cache the square root of the discriminant.
	double discriminantSqrt = sqrt(discriminant);

	// Find the nearest root that lies in the acceptable range.
	double root = (-halfB - discriminantSqrt) / a;
	bool negative = true;

	// Check if the ray hit an object along its ray direction that is closer than the previously found hit, if there is any.
	if (root < t_min || root > t_max) {
		root = (-halfB + discriminantSqrt) / a;
		if (root < t_min || root > t_max) {
			return false; // The ray may have intersected with the sphere, but it was not in our acceptable root range.
		}
		negative = false;
	}

	// Save the intersection information.
	record.t = root;
	record.position = ray.at(record.t);
	record.setFaceNormal(
	    ray, (record.position - center) / radius); // Get Normal and normalize it. Use it to determine the surface face direction (outward or inward).
	record.material = material;

	return true;
}
