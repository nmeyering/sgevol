#ifndef SGEVOL_SIMPLEX_NOISE_HPP_INCLUDED
#define SGEVOL_SIMPLEX_NOISE_HPP_INCLUDED

#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <fcppt/math/vector/vector.hpp>
#include <vector>

namespace sgevol
{

template<
	typename Float,
	std::size_t N
>
class simplex_noise
{
public:
	typedef
	typename
	fcppt::math::vector::static_< 
		Float,
		N
	>::type
	vector;

	Float
	sample(
		vector const &);

	simplex_noise<
		Float,
		N
	>(
		std::size_t const _dim);

private:
	typedef
	unsigned int
	index_type;

	typedef
	std::vector<
		index_type
	> index_container;

	// index_type index(); // wie?

	std::size_t dim_;
	index_container perm_;
	std::vector<vector> gradients_;
};

template<typename Float, std::size_t N>
simplex_noise<Float,N>::simplex_noise(
	std::size_t const _dim)
:
	dim_(
		_dim),
	perm_(
		256)
{
	boost::iota(perm_,static_cast<index_type>(0));
	boost::random_shuffle(perm_);
	for (std::size_t i = 0; i < N; ++i)
	{
		vector tmp = vector::null(); tmp[i] = 1;
		gradients_.push_back(tmp);
		tmp[i] = -1;
		gradients_.push_back(tmp);
	}
}

template<typename Float, std::size_t N>
Float
simplex_noise<Float,N>::sample(
	vector const &in)
{
	return static_cast<Float>(0.0);
}

}
#endif
