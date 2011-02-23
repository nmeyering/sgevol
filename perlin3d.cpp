#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <fcppt/random/uniform.hpp>
#include <fcppt/random/make_inclusive_range.hpp>
#include <fcppt/time/std_time.hpp>
#include <fcppt/random/default_generator.hpp>
#include <algorithm>
#include <math.h>
#include <fcppt/math/interpolation/linear.hpp>
#include <fcppt/math/interpolation/trigonometric.hpp>
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

	fcppt::random::uniform<float> rngx(
		fcppt::random::make_inclusive_range(
			0.0f,
			1.0f
		),
		fcppt::random::default_generator(
			fcppt::time::std_time()
		)
	);

	fcppt::random::uniform<float> rngy(
		fcppt::random::make_inclusive_range(
			0.0f,
			1.0f
		),
		fcppt::random::default_generator(
			fcppt::time::std_time() + 1
		)
	);

	fcppt::random::uniform<float> rngz(
		fcppt::random::make_inclusive_range(
			0.0f,
			1.0f
		),
		fcppt::random::default_generator(
			fcppt::time::std_time() + 2
		)
	);

	vec3 tmp;
	while( gradients_.size() < dim_ )
	{
		tmp = vec3(
			rngx(),
			rngy(),
			rngz()
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
				] = rngx();
				/*= gradients_[ 
					(	x + 
						perm_[
							( y +
								perm_[ z ] 
							) % dim_
						] ) % dim_
				];
				*/
}

float perlin3d::sample(
	vec3 const &point
)
{
	if(
		point.x() >= dim_ ||
		point.y() >= dim_ ||
		point.z() >= dim_	)
	return 0.f;

	typedef
	grid_type::dim
	dim3;

	vec3 floor(
		std::floor( point.x() ),
		std::floor( point.y() ),
		std::floor( point.z() ) );

	fcppt::container::array<float, 8> n_contribs;

	for( unsigned i = 0; i < 8; ++i )
	{
		vec3 neighbor(
			floor.x() +  static_cast<float>(i & 1u),
			floor.y() +  static_cast<float>(i & 2u)/2.f,
			floor.z() +  static_cast<float>(i & 4u)/4.f 
		);
		n_contribs[i] = 
			grid_[
				fcppt::math::vector::structure_cast<
					dim3
			>( neighbor )
			];
	}

	using fcppt::math::interpolation::linear;
	using fcppt::math::interpolation::trigonometric;

	vec3 const diff( point - floor );
	float const tx = trigonometric( diff.x(), 1.0f, 0.0f );
	float const ty = trigonometric( diff.y(), 1.0f, 0.0f );
	float const tz = trigonometric( diff.z(), 1.0f, 0.0f );
	
	float const x1 = linear( tx, n_contribs[1], n_contribs[0] );
	float const x2 = linear( tx, n_contribs[3], n_contribs[2] );
	float const x3 = linear( tx, n_contribs[5], n_contribs[4] );
	float const x4 = linear( tx, n_contribs[7], n_contribs[6] );
	float const y1 = linear( ty, x2, x1 );
	float const y2 = linear( ty, x4, x3 );
	return linear( tz, y2, y1 );
}
