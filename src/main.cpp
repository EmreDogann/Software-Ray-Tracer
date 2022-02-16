#include "camera.hpp"
#include "color.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "math_utils.hpp"
#include "ray.hpp"
#include "sphere.hpp"

// std
#include <atomic>
#include <chrono>
#include <cstddef>
#include <future>
#include <mutex>
#include <thread>

Vec3 rayColor(const Ray &ray, const Hittable &world, int depth) {
	HitRecord record;

	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0)
		return Vec3(0, 0, 0);

	// World plane. Colored white.
	if (world.hit(ray, 0.001, infinity, record)) {
		Ray scattered;
		Vec3 attenuation;

		if (record.material->scatter(ray, record, attenuation, scattered)) {
			return attenuation * rayColor(scattered, world, depth - 1); // Recursive Call
		}
		return Vec3(0, 0, 0); // Edges of objects.
	}

	Vec3 unit_direction = unit_vector(ray.direction());               // Get the ray's direction as a unit vector (-1 to 1).
	auto t = 0.5 * (unit_direction.y() + 1.0);                        // Map the ray's y direction range from [-1,1] to [0,1].
	return (1.0 - t) * Vec3(1.0, 1.0, 1.0) + t * Vec3(0.5, 0.7, 1.0); // Lerp between white and sky blue depending on the value of t.
}

HittableList random_scene() {
	HittableList world;

	std::shared_ptr<Lambertian> groundMaterial = std::make_shared<Lambertian>(Vec3(0.5, 0.5, 0.5));
	world.add(std::make_shared<Sphere>(Vec3(0, -1000, 0), 1000, groundMaterial));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			double materialProbability = random_double();
			Vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - Vec3(4, 0.2, 0)).length() > 0.9) {
				std::shared_ptr<Material> sphereMaterial;

				if (materialProbability < 0.8) {
					// diffuse
					Vec3 albedo = Vec3::random() * Vec3::random();
					sphereMaterial = std::make_shared<Lambertian>(albedo);
					world.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
				} else if (materialProbability < 0.95) {
					// metal
					Vec3 albedo = Vec3::random(0.5, 1);
					double fuzz = random_double(0, 0.5);
					sphereMaterial = std::make_shared<Metal>(albedo, fuzz);
					world.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
				} else {
					// glass
					sphereMaterial = std::make_shared<Dielectric>(1.5);
					world.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
				}
			}
		}
	}

	std::shared_ptr<Dielectric> materialDielectric = std::make_shared<Dielectric>(1.5);
	world.add(std::make_shared<Sphere>(Vec3(0, 1, 0), 1.0, materialDielectric));

	std::shared_ptr<Lambertian> materialLambertian = std::make_shared<Lambertian>(Vec3(0.4, 0.2, 0.1));
	world.add(std::make_shared<Sphere>(Vec3(-4, 1, 0), 1.0, materialLambertian));

	std::shared_ptr<Metal> materialMetal = std::make_shared<Metal>(Vec3(0.7, 0.6, 0.5), 0.0);
	world.add(std::make_shared<Sphere>(Vec3(4, 1, 0), 1.0, materialMetal));

	return world;
}

struct RowResult {
	unsigned int row;
	std::vector<Vec3> rayColors;
};

int main() {
	// Image
	const double aspect_ratio = 16.0 / 9.0;
	const int image_width = 2560;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samplesPerPixel = 100;
	const int maxDepth = 50;

	// World
	HittableList world = random_scene();

	// Camera
	Vec3 lookFrom(13.0f, 2.0f, 3.0f);
	Vec3 lookAt(0.0f, 0.0f, 0.0f);
	Camera camera(lookFrom, lookAt, Vec3(0, 1, 0), 30.0f, aspect_ratio, 0.1f, 10.0f);

	// Setup multi-threading.
	std::mutex mutex;
	const unsigned int threads = std::thread::hardware_concurrency();
	volatile std::atomic<std::size_t> rowCount(0);
	std::vector<std::future<std::vector<RowResult>>> future_vec;

	for (unsigned int thread = 0; thread < threads; thread++) {
		future_vec.emplace_back(std::async(std::launch::async, [=, &mutex, &world, &rowCount]() {
			auto begin = std::chrono::high_resolution_clock::now();

			std::vector<RowResult> results_vec{};
			// Traverse image from left to right, top to bottom.
			while (rowCount < image_height) {
				auto currentRow = rowCount++;
				RowResult rowResult;
				// std::cerr << "\rScanlines remaining: " << rowCount << ' ' << std::flush;
				for (int col = 0; col < image_width; ++col) {
					Vec3 pixelColor(0, 0, 0);
					for (int s = 0; s < samplesPerPixel; ++s) {
						// Map pixel coordinates from screen space to [0,1] range.
						double u = (col + random_double()) / (image_width - 1);
						double v = (currentRow + random_double()) / (image_height - 1);

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

					// Gamme Correction.
					pixelColor[0] = sqrt(pixelColor[0] / samplesPerPixel);
					pixelColor[1] = sqrt(pixelColor[1] / samplesPerPixel);
					pixelColor[2] = sqrt(pixelColor[2] / samplesPerPixel);

					rowResult.row = currentRow;
					rowResult.rayColors.emplace_back(pixelColor);

					// writeColor(imageFile, pixelColor, samplesPerPixel);
				}
				results_vec.emplace_back(rowResult);
			}

			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

			std::lock_guard<std::mutex> lock(mutex);
			std::cout << "Done!. Time measured: " << duration.count() * 1e-9 << " seconds." << std::endl;
			return results_vec;
		}));
	}

	std::vector<std::vector<Vec3>> orderedRows(image_height);
	for (auto &future : future_vec) {
		auto result = future.get();
		for (auto rows : result) {
			orderedRows[rows.row] = rows.rayColors;
		}
	}

	std::ofstream imageFile("Image.ppm");
	if (imageFile.is_open()) {

		// Render
		imageFile << "P3\n";
		imageFile << image_width << ' ' << image_height << '\n';
		imageFile << "255\n";

		for (int i = orderedRows.size() - 1; i >= 0; i--) {
			for (auto color : orderedRows[i]) {
				// Write the translated [0,255] value of each color component.
				// imageFile << 255 << ' ' << 255 << ' ' << 255 << '\n';
				imageFile << static_cast<int>(256 * clamp(color.x(), 0.0, 0.999)) << ' ' << static_cast<int>(256 * clamp(color.y(), 0.0, 0.999))
				          << ' ' << static_cast<int>(256 * clamp(color.z(), 0.0, 0.999)) << '\n';
			}
		}
	}
	imageFile.close();

	// Launch image using default image app.
	std::system("Image.ppm");
}