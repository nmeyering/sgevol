#include <fcppt/container/array.hpp>
#include <fcppt/math/clamp.hpp>
#include <fcppt/math/mod.hpp>
#include <fcppt/math/interpolation/linear.hpp>
#include <fcppt/math/interpolation/trigonometric.hpp>
#include <fcppt/random/make_inclusive_range.hpp>
#include <fcppt/random/uniform.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/range/algorithm_ext/iota.hpp>
#include <algorithm>
#include <math.h>
#include <fcppt/config/external_end.hpp>


#include "perlin2d.hpp"

using sgevollib::perlin2d;

perlin2d::vec2
perlin2d::next_gradient(
	fcppt::random::uniform<float> &rng
)
{
	vec2 tmp;
	do
	{
		tmp = vec2(
			rng(),
			rng());
	}
	while(fcppt::math::vector::length(tmp) > 1.f);
	return fcppt::math::vector::normalize(tmp);
}

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
		if (fcppt::math::vector::length( tmp ) <= 1.f)
			gradients_.push_back(
				fcppt::math::vector::normalize( tmp )
			);
	}

	for( grid_type::size_type y = 0; y < dim_; ++y )
		for( grid_type::size_type x = 0; x < dim_; ++x )
			grid_[
				grid_type::dim(
					x,
					y
				)
			] = rng();
			/*gradients_[
				(
					x +
					perm_[ y ]
				)
				% dim_
			];*/
}

float perlin2d::sample(
	vec2 const &point
)
{
	vec2 p(
		fcppt::math::mod( point.x(), static_cast<float>(dim_ )),
		fcppt::math::mod( point.y(), static_cast<float>(dim_ ))
	);
	if(
		p.x() >= dim_ ||
		p.y() >= dim_
		)
	return 0.f;

	typedef
	grid_type::dim
	dim2;

	vec2 floor(
		std::floor( p.x() ),
		std::floor( p.y() ) );

	/*
	std::cout <<
		"---------------------------------" << std::endl <<
		"point: " << point << std::endl <<
		"floor: " << floor << std::endl <<
		"---------------------------------" << std::endl;
	*/
	fcppt::container::array<float, 4> n_contribs;
	for( unsigned i = 0; i < 4; ++i )
	{
		vec2 neighbor(0.0f,0.0f);
		switch (i)
		{
			case 0:
				neighbor = vec2(
					floor.x() +  0.f,
					floor.y() +  0.f
				);
			break;
			case 1:
				neighbor = vec2(
					floor.x() +  1.f,
					floor.y() +  0.f
				);
			break;
			case 2:
				neighbor = vec2(
					floor.x() +  0.f,
					floor.y() +  1.f
				);
			break;
			case 3:
				neighbor = vec2(
					floor.x() +  1.f,
					floor.y() +  1.f
				);
		}
		float grad = grid_[
				fcppt::math::vector::structure_cast<
					dim2
			>( neighbor)
		];
		n_contribs[i] = grad;
	}
	vec2 const diff( p - floor );
	using fcppt::math::interpolation::linear;
	using fcppt::math::interpolation::trigonometric;

	float const tx = trigonometric( diff.x(), 1.0f, 0.0f);
	float const ty = trigonometric( diff.y(), 1.0f, 0.0f);
	//float const tx = diff.x();
	//float const ty = diff.y();

	float const x1 = linear( tx, n_contribs[1], n_contribs[0] );
	float const x2 = linear( tx, n_contribs[3], n_contribs[2] );
	return linear( ty, x2, x1 );
}

void
perlin2d::fill_grid(
	output_grid &grid,
	float scale
)
{
	typedef
	output_grid::size_type
	dimtype;

	dimtype len = grid.size().w();

	float factor =
		static_cast<float>(dim_) /
		static_cast<float>(len);

	for( dimtype y = 0; y < len; ++y )
		for( dimtype x = 0; x < len; ++x )
		{
			grid[ output_grid::dim(
				x,
				y
			)] =
			 scale * (
				fcppt::math::clamp(
						sample(
							vec2(
								static_cast<float>(x) * factor,
								static_cast<float>(y) * factor
							) ),
					0.0f,
					1.0f
				)
			);
		}
}
