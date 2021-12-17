#pragma once

void write_color(unsigned char*& out, const color& pixel_color, int samples_per_pixel)
{
	const auto scale = 1.0/samples_per_pixel;
	const auto r = pixel_color.r * scale;
	const auto g = pixel_color.g * scale;
	const auto b = pixel_color.b * scale;

	*out++ = static_cast<unsigned char>(256 * clamp(r, 0.0, 0.999));
	*out++ = static_cast<unsigned char>(256 * clamp(g, 0.0, 0.999));
	*out++ = static_cast<unsigned char>(256 * clamp(b, 0.0, 0.999));
}