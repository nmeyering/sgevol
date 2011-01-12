#ifndef SGEVOL_PERLIN3D_HPP_INCLUDED
#define SGEVOL_PERLIN3D_HPP_INCLUDED

#include <fcppt/math/vector/static.hpp>
#include <fcppt/container/grid/object.hpp>
#include "trig_lerp.hpp"
#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <vector>
#include <algorithm>



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
	vec3 const &foo
);

private:

typedef
unsigned int
index_type;

typedef
std::vector<index_type> 
container_type;

typedef fcppt::container::grid::object<
	index_type,
	3
> grid_type;

std::size_t dim_;

container_type perm;

grid_type grid_;

std::vector<
	vec3
> gradients_;

};
	
}

#endif
