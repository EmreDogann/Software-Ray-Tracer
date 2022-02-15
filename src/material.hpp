#pragma once

#include "hittable.hpp"
#include "vector3.hpp"

class Material {
public:
	virtual bool scatter(const Ray &rayIn, const HitRecord &record, Vec3 &attenuation, Ray &scattered) const = 0;
};

class Lambertian : public Material {
public:
	Lambertian(const Vec3 &albedo) : albedo(albedo) {}

	virtual bool scatter(const Ray &rayIn, const HitRecord &record, Vec3 &attenuation, Ray &scattered) const override {
		// Vec3 scatter_direction = record.normal + random_in_unit_sphere(); // Simple Diffuse Hack
		Vec3 scatter_direction = record.normal + random_unit_vector(); // Ideal Lambertian Diffuse
		// Vec3 scatter_direction = random_in_hemisphere(record.normal); // Ideal Lambertian Diffuse Alternative

		// Catch degenerate scatter direction
		if (scatter_direction.near_zero()) {
			scatter_direction = record.normal;
		}

		scattered = Ray(record.position, scatter_direction);
		attenuation = albedo;
		return true;
	}

private:
	Vec3 albedo;
};

class Metal : public Material {
public:
	Metal(const Vec3 &albedo, double glossy) : albedo(albedo), glossy(glossy < 1 ? glossy : 1) {}

	virtual bool scatter(const Ray &rayIn, const HitRecord &record, Vec3 &attenuation, Ray &scattered) const override {
		Vec3 reflected = reflect(unit_vector(rayIn.direction()), record.normal);
		scattered = Ray(record.position, reflected + glossy * random_in_unit_sphere());
		attenuation = albedo;
		return dot(scattered.direction(), record.normal) > 0;
	}

private:
	Vec3 albedo;
	double glossy;
};

class Dielectric : public Material {
public:
	Dielectric(double refractionIndex) : refractionIndex(refractionIndex) {}

	virtual bool scatter(const Ray &rayIn, const HitRecord &record, Vec3 &attenuation, Ray &scattered) const override {
		attenuation = Vec3(1.0f, 1.0f, 1.0f);

		Vec3 unitDirection = unit_vector(rayIn.direction());
		double refractionRatio = record.frontFace ? (1.0f / refractionIndex) : refractionIndex;

		// Test for Total Internal Reflection.
		double cos_theta = fmin(dot(-unitDirection, record.normal), 1.0);
		double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

		bool cannotRefract = refractionRatio * sin_theta > 1.0;

		Vec3 direction;
		if (cannotRefract || reflectance(cos_theta, refractionRatio) > random_double()) {
			// If cannotRefract = Total Internal Reflection.
			// If reflectance = Steep angle reflectivity.
			direction = reflect(unitDirection, record.normal);
		} else { // Refraction.
			     // Currently only works for air (index = 1) to refraction index.
			direction = refract(unitDirection, record.normal, refractionRatio);
		}

		scattered = Ray(record.position, direction);
		return true;
	}

private:
	double refractionIndex;

	static double reflectance(double cosine, double refractiveIndex) {
		// Use Schlick's approximation for reflectance.
		auto r0 = (1 - refractiveIndex) / (1 + refractiveIndex);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}
};