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

template<std::size_t N>
void
test(
	std::size_t dim, 
	std::size_t octaves)
{
	float const scaling = 64.f;
	float const offset = 128.f;
	typedef fcppt::chrono::steady_clock clock;
	typedef clock::time_point time_point;
	typedef clock::duration duration;

	sgevol::simplex_noise<float,N> noisy(dim, 256);

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

	typedef typename sgevol::simplex_noise<float,N>::vector vec;

	grid tmpgrid(
		grid::dim(
			dim,
			dim));

	time_point before = clock::now();
	vec pos;
	
	for (std::size_t oct = 0; oct < octaves; ++oct)
	for (grid::size_type y = 0; y < dim; ++y)
		for (grid::size_type x = 0; x < dim; ++x)
		{
			pos[0] = offset + scaling * std::pow(2.f, static_cast<float>(oct)) * static_cast<float>(x)/
				static_cast<float>(dim);
			pos[1] = offset + scaling * std::pow(2.f, static_cast<float>(oct)) * static_cast<float>(y)/
				static_cast<float>(dim);

			float tmp =
				noisy.sample(
					pos);

			tmpgrid[
				grid::dim(
					x,
					y)
			] += tmp * std::pow(0.5f, static_cast<float>(oct));
		}

	time_point after = clock::now();
	duration diff(after - before);

	std::cout << fcppt::chrono::duration_cast<fcppt::chrono::milliseconds>(diff) << std::endl;
}
