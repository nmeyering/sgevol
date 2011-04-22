#include <fcppt/container/grid/object.hpp>
#include <iostream>
#include <cmath>
#include "simplex_noise.hpp"
#include "to_ppm.hpp"

int main()
{
	std::size_t const dim = 512;
	std::size_t const n = 4; // dimensionality - also change sampling vector below!
	std::size_t const octaves = 5;
	float const scaling = 4.f;

	sgevol::simplex_noise<float,n> noisy(dim);

	typedef
	fcppt::container::grid::object<
		float,
		2
	>
	grid;

	typedef
	fcppt::container::grid::object<
		unsigned char,
		2
	>
	output_grid;

	typedef
	sgevol::simplex_noise<float,n>::vector
	vec;

	grid tmpgrid(
		grid::dim(
			dim,
			dim));

  using std::pow;
	for (std::size_t oct = 0; oct < octaves; ++oct)
	for (grid::size_type y = 0; y < dim; ++y)
		for (grid::size_type x = 0; x < dim; ++x)
		{
			float tmp =
				noisy.sample(
					vec(
						scaling * std::pow(2.f, static_cast<float>(oct)) * static_cast<float>(x)/
							static_cast<float>(dim),
						scaling * std::pow(2.f, static_cast<float>(oct)) * static_cast<float>(y)/
							static_cast<float>(dim),
						0.f,
						0.f)); // pad to fill {3,4} dimensions

			tmpgrid[
				grid::dim(
					x,
					y)
			] += tmp * std::pow(0.5f, static_cast<float>(oct));
		}

	output_grid output(
		output_grid::dim(
			dim,
			dim));
	
	for (output_grid::size_type y = 0; y < dim; ++y)
		for (output_grid::size_type x = 0; x < dim; ++x)
		{
			output[
				output_grid::dim(
					x,
					y)
			] = static_cast<unsigned char>(
			256 *
			(0.5f + 0.5f *
			tmpgrid[
				grid::dim(
					x,
					y
				)
			]));
		}

	to_ppm(
		output,
		std::cout,
		static_cast<unsigned char>(255));

	//std::cout << noisy.sample(vec(0.5f,0.78f)) << std::endl;
}
