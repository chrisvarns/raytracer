#include "stb_image_write.h"

#include "ray.h"
#include "vec3.h"

color ray_color(const ray& r)
{
	vec3 unit_direction = normalize(r.dir);
	auto t = 0.5*(unit_direction.y + 1);
	return (1.0-t)*color(1.0,1.0,1.0) + t*color(0.5,0.7,1.0);
}

int main()
{
	// Image
	const auto aspect_ratio = 16.0 / 9.0;
	const int image_width = 400;
	const int image_height = static_cast<int>(image_width / aspect_ratio);

	// Camera
	const auto viewport_height = 2.0;
	const auto viewport_width = aspect_ratio * viewport_height;
	const auto focal_length = 1.0;

	const auto origin = point3(0, 0, 0);
	const auto horizontal = vec3(viewport_width, 0, 0);
	const auto vertical = vec3(0, viewport_height, 0);
	const auto lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length);

	// Render
	unsigned char image_data[image_width * image_height * 3];
	unsigned char* image_data_ptr = image_data;
	for (int j = image_height - 1; j >= 0; --j)
	{
		std::cout << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i)
		{
			const auto u = double(i) / (image_width-1);
			const auto v = double(j) / (image_height-1);
			const ray r(origin, lower_left_corner + u*horizontal + v*vertical - origin);

			color pixel_color = ray_color(r);
			*image_data_ptr++ = static_cast<unsigned char>(255.999 * pixel_color.x);
			*image_data_ptr++ = static_cast<unsigned char>(255.999 * pixel_color.y);
			*image_data_ptr++ = static_cast<unsigned char>(255.999 * pixel_color.z);
		}
	}

	std::cout << "\nDone.\n";

	const char* image_filename = "image.png";
	stbi_write_png(image_filename, image_width, image_height, 3, image_data, 0);
}