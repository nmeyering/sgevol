#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <fcppt/random/uniform.hpp>
#include <fcppt/random/make_inclusive_range.hpp>
#include <algorithm>
#include <math.h>
#include <fcppt/math/lerp.hpp>
#include "trig_lerp.hpp"
#include "perlin2d.hpp"

#include <iostream>

using sgevol::perlin2d;

perlin2d::perlin2d(
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

	vec2 tmp;
	while( gradients_.size() < dim_ )
	{
		tmp = vec2(
			rng(),
			rng()
		);
		if (fcppt::math::vector::length( tmp ) <= 1.0)
			gradients_.push_back( 
				fcppt::math::vector::normalize( tmp )
			);
	}

	for( grid_type::size_type x = 0; x < dim_; ++x )
		for( grid_type::size_type y = 0; y < dim_; ++y )
			grid_[
				grid_type::dim(
					x,
					y
				)
			] = gradients_[ 
				( x + 
				perm_[
					 y % dim_ ]
				)
				% dim_
			];
}

float perlin2d::sample(
	vec2 const &point
)
{
	if(
		point.x() >= dim_ ||
		point.y() >= dim_
		)
	return 0.f;

	typedef
	grid_type::dim
	dim2;

	vec2 floor(
		std::floor( point.x() ),
		std::floor( point.y() ) );

	std::vector<float> n_contribs;
	for( unsigned i = 0; i < 4; ++i )
	{
		vec2 neighbor(
			floor.x() +  static_cast<float>(i & 1u),
			floor.y() +  static_cast<float>(i & 2u)/2.f
		);
		vec2 grad = grid_[
				fcppt::math::vector::structure_cast<
					dim2
			>( neighbor)
		];
		n_contribs.push_back(
			fcppt::math::vector::dot(
				grad,
				point - neighbor
				)
		);
	}

	vec2 const diff( point - floor );
	float const tx = trig_lerp( 0.0f, 1.0f, diff.x() );
	float const ty = trig_lerp( 0.0f, 1.0f, diff.y() );
	
	using fcppt::math::lerp;
	float const x1 = lerp( tx, n_contribs[0], n_contribs[1] );
	float const x2 = lerp( tx, n_contribs[2], n_contribs[3] );
	return lerp( ty, x1, x2 );
}

void 
perlin2d::fill_grid(
	output_grid &grid
)
{
	typedef 
	output_grid::size_type
	dimtype;

	dimtype len = grid.dimension().w();
	for( dimtype y = 0; y < len; ++y )
		for( dimtype x = 0; x < len; ++x )
		{
			grid[ output_grid::dim( 
				x,
				y
			)] =
			sample(
				vec2(
					static_cast<float>(x),
					static_cast<float>(y)
				)
			);
		}
}
