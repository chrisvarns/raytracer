#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "rtweekend.h"
#include "sphere.h"
#include "stb_image_write.h"

color ray_color(const ray& r, const hittable& world, int depth)
{
	hit_record rec;

	// if we've exceeded the ray bounce limit, no more light is gathered
	if(depth <= 0)
		return color(0,0,0);

	if (world.hit(r, 0, infinity, rec))
	{
		const point3 target = rec.p + rec.normal + random_in_unit_sphere();
		return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth-1);
	}

	const vec3 unit_direction = normalize(r.dir);
	const auto t = 0.5 * (unit_direction.y + 1.0);
	return (1.0 - t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main()
{
	// Image
	const auto aspect_ratio = 16.0 / 9.0;
	const int image_width = 400;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 128;
	const int max_depth = 50;

	// World
	hittable_list world;
	world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
	world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

	// Camera
	camera cam;

	// Render
	unsigned char image_data[image_width * image_height * 3];
	unsigned char* image_data_ptr = image_data;
	for (int j = image_height - 1; j >= 0; --j)
	{
		std::cout << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i)
		{
			color pixel_color(0,0,0);
			for (int s = 0; s < samples_per_pixel; ++s)
			{
				const auto u = (i+random_double()) / (image_width-1);
				const auto v = (j+random_double()) / (image_height-1);
				const ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, world, max_depth);
			}
			write_color(image_data_ptr, pixel_color, samples_per_pixel);
		}
	}

	std::cout << "\nDone.\n";

	const char* image_filename = "image.png";
	stbi_write_png(image_filename, image_width, image_height, 3, image_data, 0);
}