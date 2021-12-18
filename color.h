#pragma once

void write_color(unsigned char* out, const color& pixel_color, int samples_per_pixel)
{
	out[0] = static_cast<unsigned char>(256 * clamp(pixel_color.r, 0.0, 0.999));
	out[1] = static_cast<unsigned char>(256 * clamp(pixel_color.g, 0.0, 0.999));
	out[2] = static_cast<unsigned char>(256 * clamp(pixel_color.b, 0.0, 0.999));
}

void resolve_samples(color& pixel_color, int samples_per_pixel)
{
	const auto scale = 1.0 / samples_per_pixel;
	pixel_color.r = sqrt(scale * pixel_color.r);
	pixel_color.g = sqrt(scale * pixel_color.g);
	pixel_color.b = sqrt(scale * pixel_color.b);
}