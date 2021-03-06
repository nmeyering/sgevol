#ifndef SGEVOLLIB_SIMPLEX_NOISE_HPP_INCLUDED
#define SGEVOLLIB_SIMPLEX_NOISE_HPP_INCLUDED

#include <fcppt/container/array.hpp>
#include <fcppt/math/matrix/arithmetic.hpp>
#include <fcppt/math/matrix/static.hpp>
#include <fcppt/math/matrix/vector.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/math/vector/dot.hpp>
#include <fcppt/math/vector/static.hpp>
#include <fcppt/math/vector/unit.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/range/algorithm_ext/iota.hpp>
#include <cmath>
#include <cstddef>
#include <vector>
#include <fcppt/config/external_end.hpp>


namespace sgevollib
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

	simplex_noise(
		std::size_t const _dim,
		std::size_t const _width);

private:
	typedef
	fcppt::math::size_type
	index_type;

	typedef
	std::vector<
		index_type
	> index_container;

	typedef
	typename
	fcppt::math::matrix::static_<
		Float,
		N,
		N
	>::type
	matrix;

	std::size_t dim_;
	index_container perm_;
	std::vector<vector> gradients_;

	std::size_t
	mod(
	long int const &a,
	std::size_t const &b)
	{
		if (a >= 0)
			return static_cast<std::size_t>(a) %
				static_cast<std::size_t>(b);
		else
			return static_cast<std::size_t>(-a) %
				static_cast<std::size_t>(b);
	}

	std::size_t
	index(
		vector const &vec)
	{
		std::size_t res = static_cast<std::size_t>(0);
		for (typename vector::const_iterator it = vec.begin(); it != vec.end(); ++it)
		{
			long int t = static_cast<long int>(*it);
			res = perm_[mod(
				static_cast<long int>(res) +
					static_cast<long int>(t),
				perm_.size())];
		}
		return res;
	}

	typedef fcppt::container::array<
		vector,
		N + 1
	> corner_array;

	corner_array
	corners(
		vector point)
	{
		corner_array res;
		vector cur = vector::null();
		Float max = static_cast<Float>(-1);
		typename vector::size_type max_i = 0;

		res[0] = vector(cur);

		for (typename vector::size_type j = 0; j < N; ++j)
		{
			for (typename vector::size_type i = 0; i < N; ++i)
				if (point[i] > max)
				{
					max = point[i];
					max_i = i;
				}
			max = static_cast<Float>(-1);
			point[max_i] = static_cast<Float>(-2);
			cur += fcppt::math::vector::unit<vector>(max_i);
			res[j+1] = vector(cur);
		}

		return res;
	}

	Float
	stretch_factor()
	{
		return static_cast<Float>(
			1.0/(1.0 + std::sqrt(1.0 + N))
		);
	}

	Float
	inv_factor()
	{
		return static_cast<Float>(
			-1.0/(1.0 + N + std::sqrt(1.0 + N))
		);
	}

	matrix
	stretch_m()
	{
		matrix tmp;
		for (typename matrix::size_type i = 0; i < N; ++i)
			for (typename matrix::size_type j = 0; j < N; ++j)
				tmp[i][j] =
					stretch_factor() +
					((i == j)? 1 : 0 );
		return tmp;
	}

	matrix
	inv_m()
	{
		matrix tmp;
		for (typename matrix::size_type i = 0; i < N; ++i)
			for (typename matrix::size_type j = 0; j < N; ++j)
				tmp[i][j] =
					inv_factor() +
					((i == j)? 1 : 0 );
		return tmp;
	}

	Float
	contrib(
		vector const &v,
		vector const &intv)
	{
		Float t = static_cast<Float>(0.6);
		t -= fcppt::math::vector::dot(v,v);
		if (t < 0)
			return static_cast<Float>(0);
		else
			t *= t;
			return t * t * fcppt::math::vector::dot(
				gradients_[
					index(intv) % (2u * N)],
				v);
	}
};

template<typename Float, std::size_t N>
simplex_noise<Float,N>::simplex_noise(
	std::size_t const _dim,
	std::size_t const _width)
:
	dim_(
		_dim),
	perm_(
		_width)
{
	boost::iota(perm_,static_cast<index_type>(0));
	boost::random_shuffle(perm_);
	for (typename vector::size_type i = 0; i < N; ++i)
	{
		vector tmp = vector::null();
		tmp[i] = static_cast<Float>(1);
		gradients_.push_back(tmp);
		tmp[i] = static_cast<Float>(-1);
		gradients_.push_back(tmp);
	}
}

template<typename Float, std::size_t N>
Float
simplex_noise<Float,N>::sample(
	vector const &in)
{
	Float res = static_cast<Float>(0);
	vector tmp = stretch_m() * in;
	for(typename vector::iterator it = tmp.begin(); it != tmp.end(); ++it)
	{
		*it = std::floor(*it);
	}
	vector floored(tmp);
	tmp = inv_m() * tmp;
	tmp = in - tmp;
	vector offset(tmp);
	tmp = stretch_m() * tmp;

	corner_array c = corners(tmp);
	for (typename corner_array::const_iterator v = c.begin(); v != c.end(); ++v)
	{
		vector t(in - inv_m() * (floored + *v));
		res +=
			contrib(t, floored + *v);
	}

	// FIXME: replace this magic number with something sensible
	return static_cast<Float>(40.0) * res;
}

}
#endif
