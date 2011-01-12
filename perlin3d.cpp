#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <fcppt/random/uniform.hpp>
#include <fcppt/random/make_inclusive_range.hpp>
#include <algorithm>
#include "perlin3d.hpp"

#include <iostream>

using sgevol::perlin3d;

perlin3d::perlin3d(
	std::size_t const _dim
)
:
	dim_(
		_dim
		),
	perm_(
		_dim
		),
	gradients_(
		_dim
		),
	grid_(
		grid_type::dim(
			_dim,
			_dim,
			_dim
		)
	)
{
	boost::iota(perm_,static_cast<index_type>(0));
	boost::random_shuffle(perm_);

	std::cout << "[ ";
	for( unsigned int i = 0; i < perm_.size(); ++i )
		std::cout << perm_[i] << " ";
	std::cout << "]" << std::endl;

	fcppt::random::uniform<float> rng(
		fcppt::random::make_inclusive_range(
			0.0f,
			1.0f
		)
	);

	for( unsigned int i = 0; i < gradients_.size(); ++i )
		gradients_[i] = vec3(
			rng(),
			rng(),
			rng()
		);

	std::cout << gradients_[42] << std::endl;
	
}

float perlin3d::sample(
	vec3 const &point
)
{
	return 0.0f;
}
