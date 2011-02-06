#include <iostream>
#include <fcppt/math/vector/vector.hpp>
#include <fcppt/math/dim/dim.hpp>
#include <fcppt/random/uniform.hpp>
#include <fcppt/random/make_inclusive_range.hpp>
#include "trig_lerp.hpp"
#include "perlin2d.hpp"
#include "to_ppm.hpp"
#include "zip_with.hpp"
#include <boost/foreach.hpp>

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

	perlin = sgevol::perlin2d( 64 );

	perlin2d::output_grid	grid2(
		perlin2d::output_grid::dim(
			512,
			512
		)
	);
	perlin.fill_grid( grid2, 0.25f );

	perlin = sgevol::perlin2d( 128 );

	perlin2d::output_grid	grid3(
		perlin2d::output_grid::dim(
			512,
			512
		)
	);
	perlin.fill_grid( grid3, 0.125f );

	perlin2d::output_grid	grid4(
		perlin2d::output_grid::dim(
			512,
			512
		)
	);

	grid1 = zip_with<float>( grid1, grid2, std::plus<float>() );
	grid4 = zip_with<float>( grid3, grid1, std::plus<float>() );
	
	typedef
	fcppt::container::grid::object<
		unsigned int,
		2
	>
	result_type;

	result_type result(
		perlin2d::output_grid::dim(
			512,
			512
		)
	);

	result_type::iterator i = 
		result.begin();
	
	BOOST_FOREACH(
	float const &pansen,
	grid4
	)
	{
		*i++ = 
			static_cast<unsigned int>(
				255.f * pansen
			);
	}

	to_ppm(
		result,
		std::cout,
		255u
	);
}
