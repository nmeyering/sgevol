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

	for( grid_type::size_type z = 0; z < dim_; ++z )
		for( grid_type::size_type y = 0; y < dim_; ++y )
			for( grid_type::size_type x = 0; x < dim_; ++x )
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
	if(
		point.x() >= dim_ ||
		point.y() >= dim_ ||
		point.z() >= dim_
		)
	return 0.f;

	typedef
	grid_type::dim
	dim3;

	vec3 floor(
		std::floor( point.x() ),
		std::floor( point.y() ),
		std::floor( point.z() ) );

	std::vector< float > n_contribs;
	for( unsigned i = 0; i < 8; ++i )
	{
		vec3 neighbor(
			floor.x() +  static_cast<float>(i & 1u),
			floor.y() +  static_cast<float>(i & 2u)/2.f,
			floor.z() +  static_cast<float>(i & 4u)/4.f 
		);
		vec3 grad = grid_[
				fcppt::math::vector::structure_cast<
					dim3
			>( neighbor)
		];
		n_contribs.push_back(
			fcppt::math::vector::dot(
				grad,
				point - neighbor
				)
		);
	}

	vec3 const diff( point - floor );
	float const tx = trig_lerp( diff.x(), 0.0f, 1.0f );
	float const ty = trig_lerp( diff.y(), 0.0f, 1.0f );
	float const tz = trig_lerp( diff.z(), 0.0f, 1.0f );
	
	using fcppt::math::lerp;
	float const x1 = lerp( tx, n_contribs[0], n_contribs[1] );
	float const x2 = lerp( tx, n_contribs[2], n_contribs[3] );
	float const x3 = lerp( tx, n_contribs[4], n_contribs[5] );
	float const x4 = lerp( tx, n_contribs[6], n_contribs[7] );
	float const y1 = lerp( ty, x1, x2 );
	float const y2 = lerp( ty, x3, x4 );
	return lerp( tz, y1, y2 );
}
