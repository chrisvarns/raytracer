#include <iostream>
#include "stb_image_write.h"

int main()
{
	const int image_width = 256;
	const int image_height = 256;
	const char* image_filename = "image.png";

	unsigned char image_data[image_width*image_height*3];
	
	unsigned char* image_data_ptr = image_data;
	for (int j = image_height - 1; j >= 0; --j)
	{
		std::cout << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i)
		{
			auto r = double(i) / (image_width-1);
			auto g = double(j) / (image_height-1);
			auto b = 0.25;

			*image_data_ptr++ = static_cast<unsigned char>(255.999 * r);
			*image_data_ptr++ = static_cast<unsigned char>(255.999 * g);
			*image_data_ptr++ = static_cast<unsigned char>(255.999 * b);
		}
	}

	std::cout << "\nDone.\n";

	stbi_write_png(image_filename, image_width, image_height, 3, image_data, 0);
}