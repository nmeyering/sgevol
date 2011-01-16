#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <fcppt/random/uniform.hpp>
#include <fcppt/random/make_inclusive_range.hpp>
#include <algorithm>
#include <math.h>
#include "trig_lerp.hpp"
#include <fcppt/math/lerp.hpp>
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


	vec3 tmp;
	while( gradients_.size() < dim_ )
	{
		tmp = vec3(
			rng(),
			rng(),
			rng()
		);
		if (fcppt::math::vector::length( tmp ) <= 1.0)
			gradients_.push_back( 
				fcppt::math::vector::normalize( tmp )
			);
	}

	std::cout << gradients_[42] << std::endl;

	for( grid_type::size_type x = 0; x < dim_; ++x )
		for( grid_type::size_type y = 0; y < dim_; ++y )
			for( grid_type::size_type z = 0; z < dim_; ++z )
				grid_[
					grid_type::dim(
						x,
						y,
						z
					)
				] = gradients_[ 
					(	x + 
						perm_[
							( y +
								perm_[ z ] 
							) % dim_
						] ) % dim_
				];
	
}

float perlin3d::sample(
	vec3 const &point
)
{
	typedef
	grid_type::dim
	dim3;

	vec3 floor(
		std::floor( point.x() ),
		std::floor( point.y() ),
		std::floor( point.z() ) );


	std::vector< vec3 > neighbors;
	for( unsigned i = 0; i < 8; ++i )
		neighbors.push_back(
			vec3(
				floor.x() +  1.0 * (i & 1u),
				floor.y() +  1.0 * (i & 2u),
				floor.z() +  1.0 * (i & 4u) 
			)
		);

	std::vector< float > n_contribs;
	for( unsigned i = 0; i < 8; ++i )
		n_contribs.push_back(
			fcppt::math::vector::dot(
				grid_[
						fcppt::math::vector::structure_cast<
							grid_type::dim 
						>( neighbors[i] )
						],
				neighbors[i] - point
			)
		);

	vec3 const diff( point - floor );
	float const tx = trig_lerp( 0.0f, 1.0f, diff.x() );
	float const ty = trig_lerp( 0.0f, 1.0f, diff.y() );
	float const tz = trig_lerp( 0.0f, 1.0f, diff.z() );
	
	using fcppt::math::lerp;
	float const x1 = lerp( tx, n_contribs[0], n_contribs[1] );
	float const x2 = lerp( tx, n_contribs[2], n_contribs[3] );
	float const x3 = lerp( tx, n_contribs[4], n_contribs[5] );
	float const x4 = lerp( tx, n_contribs[6], n_contribs[7] );
	float const y1 = lerp( ty, x1, x2 );
	float const y2 = lerp( ty, x3, x4 );
	return lerp( tz, y1, y2 );
}
