#include <iostream>
#include <fcppt/math/vector/vector.hpp>
#include <fcppt/random/uniform.hpp>
#include <fcppt/random/make_inclusive_range.hpp>
#include "trig_lerp.hpp"
#include "perlin2d.hpp"
#include "to_ppm.hpp"

int main()
{
	double const a = 10;
	double const b = 20;
	//std::cout << std::endl << "interpolating between: " << a <<  " and " << b << ":" << std::endl ;
	//for( double t = 0; t <= 1.0; t += 0.05 )
		//std::cout << sgevol::trig_lerp<double>( a, b, t ) << std::endl;

	typedef fcppt::math::vector::static_< float, 2 >::type vec2;
	fcppt::random::uniform<float> rng(
		fcppt::random::make_inclusive_range(
			0.0f,
			1.0f
		)
	);
	sgevol::perlin2d perlin( 16 );
	for( unsigned i = 0; i < 10; ++i )
	{
		sgevol::perlin2d::vec2 p(
			4.0f * rng(),
			4.0f * rng()
		);
		//std::cout << "result: " << perlin.sample( p ) << std::endl;
	}
	using	sgevol::perlin2d;
	perlin2d::output_grid	grid(
		perlin2d::output_grid::dim(
			256,
			256
		)
	);
	perlin.fill_grid( grid );
	to_ppm(
		grid,
		std::cout,
		255u
	);
}
