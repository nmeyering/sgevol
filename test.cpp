#include <iostream>
#include "trig_lerp.hpp"
#include "perlin3d.hpp"
#include <fcppt/math/vector/vector.hpp>

int main()
{
	double const a = 10;
	double const b = 20;
	std::cout << std::endl << "interpolating between: " << a <<  " and " << b << ":" << std::endl ;
	for( double t = 0; t <= 1.0; t += 0.05 )
		std::cout << sgevol::trig_lerp<double>( a, b, t ) << std::endl;

	typedef fcppt::math::vector::static_< float, 3 >::type vec3;
	vec3 p(
		.6f,
		1.2444f,
		.021f );
	sgevol::perlin3d perlin( 3 );
	std::cout << "result: " << perlin.sample( p ) << std::endl;
}
