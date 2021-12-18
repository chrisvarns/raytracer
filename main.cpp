#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "rtweekend.h"
#include "sphere.h"

#include "OpenImageDenoise/oidn.hpp"
#include "stb_image_write.h"

color ray_color(const ray& r, const hittable& world, int depth)
{
	hit_record rec;

	// if we've exceeded the ray bounce limit, no more light is gathered
	if(depth <= 0)
		return color(0,0,0);

	if (world.hit(r, 0.001, infinity, rec))
	{
		ray scattered;
		color attenuation;
		if(rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * ray_color(scattered, world, depth-1);
	}

	const vec3 unit_direction = normalize(r.dir);
	const auto t = 0.5 * (unit_direction.y + 1.0);
	return (1.0 - t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main()
{
	// Image
	const auto aspect_ratio = 16.0 / 9.0;
	const int image_width = 800;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 24;
	const int max_depth = 50;

	// World
	hittable_list world;

	auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
	auto material_left = make_shared<dielectric>(1.5);
	auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

	world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
	world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.45, material_left));
	world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

	// Camera
	const point3 lookfrom(3,3,2);
	const point3 lookat(0,0,-1);
	const vec3 vup(0,1,0);
	const auto dist_to_focus = (lookfrom-lookat).length();
	const auto aperture = 2;
	const camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

	// Render
	color* color_buffer = (color*)malloc(image_width * image_height * sizeof(color));
	color* color_buffer_pointer = color_buffer;
	for (int j = image_height - 1; j >= 0; --j)
	{
		std::cout << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i, ++color_buffer_pointer)
		{
			*color_buffer_pointer = color(0,0,0);
			for (int s = 0; s < samples_per_pixel; ++s)
			{
				const auto u = (i+random_float()) / (image_width-1);
				const auto v = (j+random_float()) / (image_height-1);
				const ray r = cam.get_ray(u, v);
				*color_buffer_pointer += ray_color(r, world, max_depth);
			}

			resolve_samples(*color_buffer_pointer, samples_per_pixel);
		}
	}

	std::cout << "\nDone.\nDenoising... ";

	color* denoised_buffer = (color*)malloc(image_width * image_height * sizeof(color));

	oidn::DeviceRef device = oidn::newDevice();
	device.commit();
	oidn::FilterRef filter = device.newFilter("RT");
	filter.setImage("color", color_buffer, oidn::Format::Float3, image_width, image_height);
	filter.setImage("output", denoised_buffer, oidn::Format::Float3, image_width, image_height);
	filter.commit();
	filter.execute();

	std::cout << "Done.\nWriting files... ";

	unsigned char* rgb888_buffer = (unsigned char*)malloc(image_width * image_height * 3);
	unsigned char* rgb888_buffer_ptr = rgb888_buffer;
	color_buffer_pointer = color_buffer;

	// First write the pre-denoised
	for (int i = 0; i < image_width * image_height; ++i, ++color_buffer_pointer, rgb888_buffer_ptr += 3)
	{
		write_color(rgb888_buffer_ptr, *color_buffer_pointer, samples_per_pixel);
	}
	stbi_write_png("predenoise.png", image_width, image_height, 3, rgb888_buffer, 0);

	// Then write the denoised
	rgb888_buffer_ptr = rgb888_buffer;
	color_buffer_pointer = denoised_buffer;
	for (int i = 0; i < image_width * image_height; ++i, ++color_buffer_pointer, rgb888_buffer_ptr += 3)
	{
		write_color(rgb888_buffer_ptr, *color_buffer_pointer, samples_per_pixel);
	}
	stbi_write_png("postdenoise.png", image_width, image_height, 3, rgb888_buffer, 0);

	std::cout << "Done.\n";
}