#pragma once

void write_color(unsigned char*& out, const color& pixel_color, int samples_per_pixel)
{
	const auto scale = 1.0/samples_per_pixel;
	const auto r = sqrt(scale * pixel_color.r);
	const auto g = sqrt(scale * pixel_color.g);
	const auto b = sqrt(scale * pixel_color.b);

	*out++ = static_cast<unsigned char>(256 * clamp(r, 0.0, 0.999));
	*out++ = static_cast<unsigned char>(256 * clamp(g, 0.0, 0.999));
	*out++ = static_cast<unsigned char>(256 * clamp(b, 0.0, 0.999));
}