#ifndef SGEVOLLIB_SIMPLEX2D_HPP_INCLUDED
#define SGEVOLLIB_SIMPLEX2D_HPP_INCLUDED

#include <fcppt/math/vector/vector.hpp>
#include <fcppt/config/external_begin.hpp>
#include <vector>
#include <fcppt/config/external_end.hpp>


namespace sgevollib
{

class simplex2d
{
public:
typedef
fcppt::math::vector::static_<
	float,
	2
>::type
vec2;

float
sample(
	vec2 const &);

simplex2d(
	std::size_t const _dim);

private:
typedef
unsigned int
index_type;

typedef
std::vector<
	index_type
> index_container;

std::size_t dim_;
index_container perm_;
std::vector<vec2> gradients_;
};

}
#endif
