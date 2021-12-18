#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "moving_sphere.h"
#include "rtweekend.h"
#include "sphere.h"

#include "OpenImageDenoise/oidn.hpp"
#include "stb_image_write.h"

#include <chrono>
#include <mutex>
#include <queue>
#include <thread>

color ray_world_albedo(const ray& r)
{
	const vec3 unit_direction = normalize(r.dir);
	const auto t = 0.5 * (unit_direction.y + 1.0);
	return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

color ray_albedo(const ray& r, const hittable& world)
{
	hit_record rec;
	if (world.hit(r, 0.001, infinity, rec))
	{
		return rec.mat_ptr->get_albedo(rec.u, rec.v, rec.p);
	}

	return ray_world_albedo(r);
}

color ray_color(const ray& r, const hittable& world, int depth)
{
	hit_record rec;

	// if we've exceeded the ray bounce limit, no more light is gathered
	if (depth <= 0)
		return color(0, 0, 0);

	if (world.hit(r, 0.001, infinity, rec))
	{
		ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * ray_color(scattered, world, depth - 1);
	}

	return ray_world_albedo(r);
}

hittable_list random_scene()
{
	hittable_list world;

	const auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
	world.add(make_shared<sphere>(point3(0, -1000,0), 1000, make_shared<lambertian>(checker)));

	const auto mat_dielectric = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, mat_dielectric));
	const auto mat_lambertian = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, mat_lambertian));
	const auto mat_metal = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, mat_metal));

	for(int a = -11; a < 11; a++) {
		for(int b = -11; b < 11; b++) {
			const auto choose_mat = random_float();
			point3 center(a + 0.9*random_float(), 0.2, b + 0.9*random_float());
			if ((center - point3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8)
				{
					// diffuse
					const auto albedo = color::random() * color::random();
					const auto center2 = center + vec3(0, random_float(0,0.5), 0);
					const auto sphere_material = make_shared<lambertian>(albedo);
					world.add(make_shared<moving_sphere>(center, center2, 0.0, 1.0, 0.2, sphere_material));
				} else if (choose_mat < 0.95) {
					// metal
					const auto albedo = color::random(0.5, 1);
					const auto fuzz = random_float(0, 0.5);
					const auto sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				} else {
					// glass
					world.add(make_shared<sphere>(center, 0.2, mat_dielectric));
				}
			}
		}
	}

	return world;
}

hittable_list two_spheres()
{
	hittable_list objects;

	auto lambert = make_shared<lambertian>(make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9)));
	objects.add(make_shared<sphere>(point3(0, -10, 0), 10, lambert));
	objects.add(make_shared<sphere>(point3(0, 10, 0), 10, lambert));

	return objects;
}

int main()
{
	// Image
	const auto aspect_ratio = 16.0 / 9.0;
	const int image_width = 1200;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 32;
	const int max_depth = 50;

	// World
	hittable_list world;
	point3 lookfrom, lookat;
	float vfov = 40.0;
	float aperture = 0;
	switch (0) {
		case 1:
			world = random_scene();
			lookfrom = point3(13,2,3);
			lookat = point3(0,0,0);
			vfov = 20;
			aperture = 0.1;
			break;
		default:
		case 2:
			world = two_spheres();
			lookfrom = point3(13,2,3);
			lookat = point3(0,0,0);
			vfov = 20;
			break;
	}

	// Camera
	const vec3 vup(0,1,0);
	const auto dist_to_focus = 10;
	const camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0.0, 0.1);

	// Buffers
	color* color_buffer = (color*)malloc(image_width * image_height * sizeof(color));
	color* albedo_ms_buffer = (color*)malloc(image_width * image_height * sizeof(color));

	// Queue of jobs (rows to do)
	std::queue<int> remaining_rows;
	for (int i = 0; i < image_height; i++)
	{
		remaining_rows.push(i);
	}

	// Task the threads will do:
	std::mutex queue_mutex;
	auto thread_task = [&]()
	{
		while (true)
		{
			int row_idx;
			{
				std::unique_lock<std::mutex> lock(queue_mutex);
				if (remaining_rows.empty()) return;
				row_idx = remaining_rows.front();
				remaining_rows.pop();
			}

			for (int i = 0; i < image_width; ++i)
			{
				const int buffer_idx = image_width * (image_height - 1 - row_idx) + i;

				color& sampled_color = color_buffer[buffer_idx];
				color& albedo_ms = albedo_ms_buffer[buffer_idx];
				sampled_color = albedo_ms = color();

				for (int s = 0; s < samples_per_pixel; ++s)
				{
					const auto u = (i + random_float()) / (image_width - 1);
					const auto v = (row_idx + random_float()) / (image_height - 1);
					const ray r = cam.get_ray(u, v);

					albedo_ms += ray_albedo(r, world);
					sampled_color += ray_color(r, world, max_depth);
				}

				resolve_samples(sampled_color, samples_per_pixel);
				resolve_samples(albedo_ms, samples_per_pixel);
			}
		}
	};

	// Spawn worker threads
	std::vector<std::thread> threads;
	auto num_threads = std::thread::hardware_concurrency();
	for (auto i = num_threads -1; i > 0; i--)
	{
		threads.push_back(std::thread(thread_task));
	}
	// Print num remaining while we wait
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		size_t num_remaining;
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			num_remaining = remaining_rows.size();
		}
		if (num_remaining == 0) break;
		std::cout << "\rScanlines remaining: " << num_remaining << ' ' << std::flush;
	}
	for (auto i = 0; i < threads.size(); i++)
	{
		threads[i].join();
	}

	std::cout << "\nDone.\nDenoising... ";

	color* denoised_buffer = (color*)malloc(image_width * image_height * sizeof(color));

	oidn::DeviceRef device = oidn::newDevice();
	device.commit();

	// prefilter the albedo
	oidn::FilterRef albedofilter = device.newFilter("RT");
	albedofilter.setImage("albedo", albedo_ms_buffer, oidn::Format::Float3, image_width, image_height);
	albedofilter.setImage("output", albedo_ms_buffer, oidn::Format::Float3, image_width, image_height);
	albedofilter.set("srgb", true);
	albedofilter.commit();
	albedofilter.execute();

	// prefilter the normal
	/*oidn::FilterRef normalfilter = device.newFilter("RT");
	normalfilter.setImage("normal", normal_ms_buffer, oidn::Format::Float3, image_width, image_height);
	normalfilter.setImage("output", normal_ms_buffer, oidn::Format::Float3, image_width, image_height);
	normalfilter.commit();
	normalfilter.execute();*/

	// filter the color
	oidn::FilterRef filter = device.newFilter("RT");
	filter.setImage("color", color_buffer, oidn::Format::Float3, image_width, image_height);
	filter.setImage("albedo", albedo_ms_buffer, oidn::Format::Float3, image_width, image_height);
	filter.setImage("output", denoised_buffer, oidn::Format::Float3, image_width, image_height);
	filter.set("cleanAux", true);
	filter.set("srgb", true);
	filter.commit();
	filter.execute();

	std::cout << "Done.\nWriting files... ";

	unsigned char* rgb888_buffer = (unsigned char*)malloc(image_width * image_height * 3);

	// First write the pre-denoised
	for (int i = 0; i < image_width * image_height; ++i)
	{
		write_color(rgb888_buffer + i*3, color_buffer[i], samples_per_pixel);
	}
	stbi_write_png("predenoise.png", image_width, image_height, 3, rgb888_buffer, 0);

	// Then write the denoised
	filter.execute();
	for (int i = 0; i < image_width * image_height; ++i)
	{
		write_color(rgb888_buffer + i*3, denoised_buffer[i], samples_per_pixel);
	}
	stbi_write_png("postdenoise.png", image_width, image_height, 3, rgb888_buffer, 0);

	std::cout << "Done.\n";
}