#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/foreach.hpp>
#include <fcppt/math/vector/dot.hpp>
#include <cmath>
#include <iostream>
#include "simplex2d.hpp"

using sgevol::simplex2d;

simplex2d::simplex2d(
	std::size_t const _dim)
:
	dim_(
		_dim),
	perm_(
		_dim)
{
	boost::iota(perm_,static_cast<index_type>(0));
	boost::random_shuffle(perm_);
	gradients_.push_back(vec2(-1.f, 0.f));
	gradients_.push_back(vec2(0.f, -1.f));
	gradients_.push_back(vec2(1.f, 0.f));
	gradients_.push_back(vec2(0.f, 1.f));
}

// 2D simplex noise
float
simplex2d::sample(
	vec2 const &in)
{
	float n0, n1, n2; // Noise contributions from the three corners

	float const skew_factor = static_cast<float>(0.5f*(sqrt(3.0f)-1.0f));
	float const inverse_factor = static_cast<float>((3.0f-sqrt(3.0f))/6.0f);

	using std::sqrt;
	using std::floor;
	using fcppt::math::vector::dot;

	float xin = in[0];
	float yin = in[1];

	float s = (xin+yin)*skew_factor;
	unsigned int i = floor(xin+s);
	unsigned int j = floor(yin+s);
	float t = (i+j)*inverse_factor;
	float X0 = i-t; // Unskew the cell origin back to (x,y) space
	float Y0 = j-t;
	float x0 = xin-X0; // The x,y distances from the cell origin
	float y0 = yin-Y0;
	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	unsigned int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
	if (x0 > y0)
	{
		// lower triangle, XY order: (0,0)->(1,0)->(1,1)
		i1 = 1;
		j1 = 0;
	}
	else 
	{
		i1 = 0;
		j1 = 1;
	}
	// upper triangle, YX order: (0,0)->(0,1)->(1,1)
	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6
	float x1 = x0 - i1 + inverse_factor; // Offsets for middle corner in (x,y) unskewed coords
	float y1 = y0 - j1 + inverse_factor;
	float x2 = x0 - 1.0f + 2.0f * inverse_factor; // Offsets for last corner in (x,y) unskewed coords
	float y2 = y0 - 1.0f+ 2.0f * inverse_factor;

	// Work out the hashed gradient indices of the three simplex corners
	unsigned int ii = i & 255;
	unsigned int jj = j & 255;
	unsigned int gi0 = perm_[ii+perm_[jj] % 256] % 4;
	unsigned int gi1 = perm_[ii+i1+perm_[jj+j1] % 256] % 4;
	unsigned int gi2 = perm_[ii+1+perm_[jj+1] % 256] % 4;
	// Calculate the contribution from the three corners
	float t0 = 0.5f - x0*x0-y0*y0;
	if(t0<0.f) n0 = 0.0f;
	else
	{
		t0 *= t0;
		n0 = t0 * t0 * dot(gradients_.at(gi0), vec2(x0, y0)); // (x,y) of grad3 used for 2D gradient
	}
	float t1 = 0.5f - x1*x1-y1*y1;
	if (t1<0)
		n1 = 0.0f;
	else
	{
		t1 *= t1;
		n1 = t1 * t1 * dot(gradients_.at(gi1), vec2(x1, y1));
	}
	float t2 = 0.5f - x2*x2-y2*y2;
	if (t2<0)
		n2 = 0.0f;
	else
	{
		t2 *= t2;
		n2 = t2 * t2 * dot(gradients_.at(gi2), vec2(x2, y2));
	}

	float ret = 70.f * (n0 + n1 + n2);
	if (ret < -1.f || ret > 1.f)
	{
		std::cerr << "x1, y1:" << "\t" << x1 << "\t" << y1 << "\n";
		std::cerr << "gi1\t" << gradients_[gi1] << "\n";
		std::cerr << "gradients_[gi1]\t" << gradients_[gi1] << "\n";
		std::cerr << "dot(gradients_[gi1], vec2(x1, y1))\t" << dot(gradients_[gi1], vec2(x1, y1)) << "\n";
		std::cerr << "t0-2:" << "\t" << t0 << "\t" << t1 << "\t" << t2 << "\n";
		std::cerr << "n0-2:" << "\t" << n0 << "\t" << n1 << "\t" << n2 << "\n";
		std::cerr << "result: " << ret << "\n";
	}
	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return ret;
}
