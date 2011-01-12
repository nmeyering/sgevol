#ifndef SGEVOL_PERLIN3D_HPP_INCLUDED
#define SGEVOL_PERLIN3D_HPP_INCLUDED

#include <fcppt/math/vector/vector.hpp>
#include <fcppt/container/grid/object.hpp>
#include <vector>

namespace sgevol
{

class perlin3d
{
public:

typedef 
fcppt::math::vector::static_< 
	float,
	3
>::type
vec3;

perlin3d(
	std::size_t const _dim
);

float sample(
	vec3 const &point
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
	vec3
> vec3_container;

typedef fcppt::container::grid::object<
	vec3,
	3
> grid_type;

std::size_t dim_;

index_container perm_;

grid_type grid_;

vec3_container gradients_;

};
	
}

#endif
