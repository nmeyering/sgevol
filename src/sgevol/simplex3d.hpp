#ifndef SGEVOL_SIMPLEX3D_HPP_INCLUDED
#define SGEVOL_SIMPLEX3D_HPP_INCLUDED

#include <fcppt/math/vector/vector.hpp>
#include <vector>

namespace sgevol
{

class simplex3d
{
public:
typedef 
fcppt::math::vector::static_< 
	float,
	3
>::type
vec3;

float
sample(
	vec3 const &);

simplex3d(
	std::size_t const _dim);

private:
typedef
unsigned int
index_type;

typedef
std::vector<
	index_type
> index_container;

unsigned int index(
  unsigned int,
  unsigned int,
  unsigned int);

std::size_t dim_;
index_container perm_;
std::vector<vec3> gradients_;
};

}
#endif