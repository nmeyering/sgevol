#ifndef SGEVOL_SIMPLEX_NOISE_HPP_INCLUDED
#define SGEVOL_SIMPLEX_NOISE_HPP_INCLUDED

#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <fcppt/math/vector/vector.hpp>
#include <fcppt/math/matrix/matrix.hpp>
#include <cmath>
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

	simplex_noise(
		std::size_t const _dim);

private:
	typedef
	unsigned int
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
	unsigned int const &b)
	{
		if (a >= 0)
			return a % b;
		else 
			return (-a) % b;
	}

	index_type
	index(
		vector const &vec)
	{
		index_type res = static_cast<index_type>(0);
		for (typename vector::const_iterator it = vec.begin(); it != vec.end(); ++it)
		{
			long int t = static_cast<long int>(*it);
			res = perm_.at(mod((res + t), 256u));
		}
		return res;
	}

	vector
	unit(
		typename vector::size_type const &n)
	{
		vector tmp = vector::null();
		tmp[n] = static_cast<
			typename vector::size_type>(1);
		return tmp;
	}

	std::vector<
		vector>
	corners(
		vector point)
	{
		std::vector<vector> res;
		vector cur = vector::null();
		Float max = static_cast<Float>(-1);
		std::size_t max_i = 0;

		res.push_back(vector(cur));

		for (std::size_t j = 0; j < N; ++j)
		{
			for (std::size_t i = 0; i < N; ++i)
				if (point[i] > max)
				{
					max = point[i];
					max_i = i;
				}
			max = static_cast<Float>(-1);
			point[max_i] = static_cast<Float>(-2);
			cur += unit(max_i);
			res.push_back(vector(cur));
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
		// DEBUG
		// std::cout << "called index() with: " << intv[0] << "\t" << intv[1] << std::endl;
		// std::cout << "index() returned: " << index(intv) << std::endl;
		if (t < 0) 
			return static_cast<Float>(0);
		else
			return t * t * t * t * fcppt::math::vector::dot(
				gradients_.at(
					index(intv) % gradients_.size()),
				v);
	}
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
		vector tmp = vector::null();
		tmp[i] = 1;
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
	Float res = static_cast<Float>(0);
	vector tmp = stretch_m() * in;
	BOOST_FOREACH(typename vector::reference i, tmp)
	{
		i = std::floor(i);
	}
	vector floored(tmp);
	tmp = inv_m() * tmp;
	tmp = in - tmp;
	vector offset(tmp);
	tmp = stretch_m() * tmp;
	/*
	BOOST_FOREACH(typename vector::reference i, tmp )
	{
		std::cout << i  << "\t";
	}
	std::cout << std::endl;
	*/

	std::vector<vector> c = corners(tmp);
	BOOST_FOREACH(vector &v, c)
	{
		vector t(in - inv_m() * (floored + v));
		/*
		BOOST_FOREACH(typename vector::reference i, v)
		{
			std::cout << i << "\t";
		}
		std::cout << std::endl;
		*/
		res +=
			contrib(t, floored + v);
	}

	return 23.f * res;
}

}
#endif
