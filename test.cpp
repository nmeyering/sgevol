#include <iostream>
#include <fcppt/math/vector/vector.hpp>
#include <fcppt/math/dim/dim.hpp>
#include <fcppt/random/uniform.hpp>
#include <fcppt/random/make_inclusive_range.hpp>
#include "trig_lerp.hpp"
#include "perlin2d.hpp"
#include "to_ppm.hpp"
#include "zip_with.hpp"

int main()
{

	typedef fcppt::math::vector::static_< float, 2 >::type vec2;
	fcppt::random::uniform<float> rng(
		fcppt::random::make_inclusive_range(
			0.0f,
			1.0f
		)
	);

	using	sgevol::perlin2d;

	sgevol::perlin2d perlin( 16 );

	perlin2d::output_grid	grid1(
		perlin2d::output_grid::dim(
			512,
			512
		)
	);
	perlin.fill_grid( grid1, 0.5f );

	perlin = sgevol::perlin2d( 32 );

	perlin2d::output_grid	grid2(
		perlin2d::output_grid::dim(
			512,
			512
		)
	);
	perlin.fill_grid( grid2, 0.25f );

	perlin = sgevol::perlin2d( 64 );

	perlin2d::output_grid	grid3(
		perlin2d::output_grid::dim(
			512,
			512
		)
	);

	perlin.fill_grid( grid3, 0.25f );

	perlin2d::output_grid	grid4(
		perlin2d::output_grid::dim(
			512,
			512
		)
	);

	grid3 = zip_with<unsigned int>( grid1, grid2, std::plus<float>() );
	grid4 = zip_with<unsigned int>( grid3, grid4, std::plus<float>() );

	to_ppm(
		grid3,
		std::cout,
		255u
	);
}
