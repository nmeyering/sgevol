#ifndef SGEVOL_PERLIN2D_HPP_INCLUDED
#define SGEVOL_PERLIN2D_HPP_INCLUDED

#include <fcppt/math/vector/vector.hpp>
#include <fcppt/container/grid/object.hpp>
#include <vector>

namespace sgevol
{

class perlin2d
{
public:

typedef 
fcppt::math::vector::static_< 
	float,
	2
>::type
vec2;

typedef
fcppt::container::grid::object<
	unsigned int,
	2
>
output_grid;

perlin2d(
	std::size_t const _dim
);

float sample(
	vec2 const &point
);

void fill_grid(
	output_grid &grid,
	float scale
);

private:

typedef
unsigned int
index_type;

typedef
std::vector<
	index_type
> index_container;

typedef
std::vector<
	vec2
> vec2_container;

typedef fcppt::container::grid::object<
	vec2,
	2
> grid_type;

std::size_t dim_;

index_container perm_;

grid_type grid_;

vec2_container gradients_;

vec2 next_gradient(
	fcppt::random::uniform<float> &rng );

};
	
}

#endif
