#include "math_utils.hpp"

#include "camera.hpp"
#include "color.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "vector3.hpp"

Vec3 rayColor(const Ray &ray, const Hittable &world, int depth) {
	HitRecord record;

	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0)
		return Vec3(0, 0, 0);

	// World plane. Colored white.
	if (world.hit(ray, 0.001, infinity, record)) {
		// Vec3 target = record.position + record.normal + random_in_unit_sphere(); // Simple Diffuse Hack
		// Vec3 target = record.position + record.normal + random_unit_vector(); // Ideal Lambertian Diffuse
		Vec3 target = record.position + random_in_hemisphere(record.normal);                     // Ideal Lambertian Diffuse Alternative
		return 0.5 * rayColor(Ray(record.position, target - record.position), world, depth - 1); // Recursive call
	}

	Vec3 unit_direction = unit_vector(ray.direction());               // Get the ray's direction as a unit vector (-1 to 1).
	auto t = 0.5 * (unit_direction.y() + 1.0);                        // Map the ray's y direction range from [-1,1] to [0,1].
	return (1.0 - t) * Vec3(1.0, 1.0, 1.0) + t * Vec3(0.5, 0.7, 1.0); // Lerp between white and sky blue depending on the value of t.
}

int main() {
	// Image
	const double aspect_ratio = 16.0 / 9.0;
	const int image_width = 1280;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samplesPerPixel = 10;
	const int maxDepth = 50;

	// World
	HittableList world;
	world.add(std::make_shared<Sphere>(Vec3(0, 0, -1), 0.5));
	world.add(std::make_shared<Sphere>(Vec3(0, -100.5, -1), 100));

	// Camera
	Camera camera;

	std::ofstream imageFile("Image.ppm");
	if (imageFile.is_open()) {
		// Render
		imageFile << "P3\n";
		imageFile << image_width << ' ' << image_height << '\n';
		imageFile << "255\n";

		// Traverse image from left to right, top to bottom.
		for (int y = image_height - 1; y >= 0; --y) {
			std::cerr << "\rScanlines remaining: " << y << ' ' << std::flush;
			for (int x = 0; x < image_width; ++x) {
				Vec3 pixelColor(0, 0, 0);
				for (int s = 0; s < samplesPerPixel; ++s) {
					// Map pixel coordinates from screen space to [0,1] range.
					double u = (x + random_double()) / (image_width - 1);
					double v = (y + random_double()) / (image_height - 1);

					/* Guide for how the ray origin and direction is determined:
					 (-x,y)                   (x,y)
					    1-----------------------2
					    |                       |
					    |                       |
					    |           X           |
					    |         (0,0)         |
					    |                       |
					    3-----------------------4
					 (x,-y)                  (-x,-y)

					    2x = horizontalSize
					    2y = verticalSize
					*/
					Ray ray = camera.getRay(u, v);
					pixelColor += rayColor(ray, world, maxDepth);
				}
				writeColor(imageFile, pixelColor, samplesPerPixel);
			}
		}

		std::cerr << "\nDone.\n";
	}
	imageFile.close();

	// Launch image using default image app.
	std::system("Image.ppm");
}