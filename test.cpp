#include <sge/time/time.hpp>
#include <fcppt/chrono/chrono.hpp>
#include <fcppt/container/grid/object.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <cmath>
#include <cstddef>
#include "simplex_noise.hpp"
#include "simplex3d.hpp"
#include "to_ppm.hpp"

int main(int argc, char **argv)
{
	std::size_t dim;
	std::size_t n;
	std::size_t octaves;
	if (argc == 4)
	{
		dim = boost::lexical_cast<std::size_t>(argv[1]);
		n = boost::lexical_cast<std::size_t>(argv[2]);
		octaves = boost::lexical_cast<std::size_t>(argv[3]);
	}
	else
	{
		dim = 512;
		n = 3;
		octaves = 2;
	}
	float const scaling = 64.f;
	float const offset = 128.f;
	typedef fcppt::chrono::steady_clock clock;
	typedef clock::time_point time_point;
	typedef clock::duration duration;

	sgevol::simplex_noise<float,3> noisy(dim, 256);
	if (n == 2)
		sgevol::simplex_noise<float,2> noisy(dim, 256);
	if (n == 4)
		sgevol::simplex_noise<float,4> noisy(dim, 256);
	// sgevol::simplex3d noisy(dim);

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

	typedef sgevol::simplex_noise<float,2>::vector vec2;
	typedef sgevol::simplex_noise<float,3>::vector vec3;
	typedef sgevol::simplex_noise<float,4>::vector vec4;

	grid tmpgrid(
		grid::dim(
			dim,
			dim));

	std::cout << "generating " << octaves << " octaves took: ";
	time_point before = clock::now();
	
	vec3 pos;
	if (n == 2)
		vec2 pos;
	if (n == 4)
		vec4 pos;

	for (std::size_t oct = 0; oct < octaves; ++oct)
	for (grid::size_type y = 0; y < dim; ++y)
		for (grid::size_type x = 0; x < dim; ++x)
		{
			if (n == 2)
				vec2 pos(offset + scaling * std::pow(2.f, static_cast<float>(oct)) * static_cast<float>(x)/
					static_cast<float>(dim),
				offset + scaling * std::pow(2.f, static_cast<float>(oct)) * static_cast<float>(y)/
					static_cast<float>(dim));
			if (n == 3)
				vec3 pos(offset + scaling * std::pow(2.f, static_cast<float>(oct)) * static_cast<float>(x)/
					static_cast<float>(dim),
				offset + scaling * std::pow(2.f, static_cast<float>(oct)) * static_cast<float>(y)/
					static_cast<float>(dim),
				0.f);
			if (n == 4)
				vec4 pos(offset + scaling * std::pow(2.f, static_cast<float>(oct)) * static_cast<float>(x)/
					static_cast<float>(dim),
				offset + scaling * std::pow(2.f, static_cast<float>(oct)) * static_cast<float>(y)/
					static_cast<float>(dim),
				0.f,
				0.f);
			float tmp =
				noisy.sample(
					pos); // pad to fill {3,4} dimensions

			tmpgrid[
				grid::dim(
					x,
					y)
			] += tmp * std::pow(0.5f, static_cast<float>(oct));
		}

	time_point after = clock::now();
	duration diff(after - before);
	std::cout << fcppt::chrono::duration_cast<fcppt::chrono::milliseconds>(diff) << "ms" << std::endl;

	/*
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
	*/

	//std::cout << noisy.sample(vec(0.5f,0.78f)) << std::endl;
}
