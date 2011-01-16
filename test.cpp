#include <iostream>
#include <fcppt/math/vector/vector.hpp>
#include <fcppt/math/mod.hpp>
#include <fcppt/random/uniform.hpp>
#include <fcppt/random/make_inclusive_range.hpp>
#include "trig_lerp.hpp"
#include "perlin2d.hpp"
#include "to_ppm.hpp"

int main()
{
	/*
	float const a = 0.;
	float const b = 1.;
	std::cout << std::endl << "interpolating between: " << a <<  " and " << b << ":" << std::endl ;
	for( float t = 0.f; t <= 1.0f; t += 0.05f )
		std::cout << sgevol::trig_lerp<float>( t, a, b ) << std::endl;

	*/
	typedef fcppt::math::vector::static_< float, 2 >::type vec2;
	fcppt::random::uniform<float> rng(
		fcppt::random::make_inclusive_range(
			0.0f,
			1.0f
		)
	);

	sgevol::perlin2d perlin( 32 );
	
	/*
	float min = 0.f, max = 0.f;
	for( unsigned i = 0; i < 2; ++i )
	{
		sgevol::perlin2d::vec2 p(
			32.0f * rng(),
			32.0f * rng()
		);
		std::cout << "result: " << perlin.sample( p ) << std::endl;
		max = ( perlin.sample(p) > max )? perlin.sample(p) : max;
		min = ( perlin.sample(p) < min )? perlin.sample(p) : min;
	}
	std::cout << 
		"min: " << min << std::endl <<
		"max: " << max << std::endl;
		*/
	
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
