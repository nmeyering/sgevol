#include <fcppt/container/grid/object.hpp>
#include <iostream>
#include <cmath>
#include "simplex2d.hpp"
#include "simplex3d.hpp"
#include "to_ppm.hpp"

int main()
{
	using sgevol::simplex2d;
	using sgevol::simplex3d;

	std::size_t const dim = 256;
	simplex2d s(dim);
  simplex3d foo(dim);

	typedef
	fcppt::container::grid::object<unsigned char,2>
	grid;

	grid noise(
		grid::dim(
			dim,
			dim));

  using std::pow;
  for (grid::size_type oct = 0; oct < 5; ++oct)
    for (grid::size_type y = 0; y < dim; ++y)
      for (grid::size_type x = 0; x < dim; ++x)
      {
        float tmp =
          foo.sample(simplex3d::vec3(
          static_cast<float>(x)/((static_cast<float>(dim)/pow(2,oct))),
          static_cast<float>(y)/((static_cast<float>(dim)/pow(2,oct))),
          0.f));

        noise[
          grid::dim(
            x,
            y)
        ] += static_cast<unsigned char>( 128 + 127 * (tmp/pow(2,oct)) );
      }

	to_ppm(
		noise,
		std::cout,
		static_cast<unsigned char>(255));
	// std::cout << s.sample(simplex2d::vec2(99.f,59.f)) << std::endl;
}
