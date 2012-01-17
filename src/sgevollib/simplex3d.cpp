#include <fcppt/math/vector/dot.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/foreach.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/range/algorithm_ext/iota.hpp>
#include <cmath>
#include <iostream>
#include <fcppt/config/external_end.hpp>


#include "simplex3d.hpp"

using sgevollib::simplex3d;

simplex3d::simplex3d(
	std::size_t const _dim)
:
	dim_(
		_dim),
	perm_(
		256)
{
	boost::iota(perm_,static_cast<index_type>(0));
	boost::random_shuffle(perm_);
	gradients_.push_back(vec3(0.f, 0.f, 1.f));
	gradients_.push_back(vec3(0.f, 1.f, 0.f));
	gradients_.push_back(vec3(1.f, 0.f, 0.f));

	gradients_.push_back(vec3(0.f, 0.f, -1.f));
	gradients_.push_back(vec3(0.f, -1.f, 0.f));
	gradients_.push_back(vec3(-1.f, 0.f, 0.f));
}

unsigned int
simplex3d::index(
	unsigned int x,
	unsigned int y,
	unsigned int z
)
{
	return perm_.at(
		(x+perm_.at(
			(y+perm_.at(z % 256)) //256 = len(perm_)
				% 256))
					% 256) % 6; //6 = len(gradients_)
}

float
simplex3d::sample(
	vec3 const &in)
{
	float n0, n1, n2, n3; // Noise contributions from the four corners
	float xin = in.x();
	float yin = in.y();
	float zin = in.z();

	using std::floor;
	using fcppt::math::vector::dot;

	float F3 = 1.0/3.0;
	float s = (xin+yin+zin)*F3; // Very nice and simple skew factor for 3D
	unsigned int i = floor(xin+s);
	unsigned int j = floor(yin+s);
	unsigned int k = floor(zin+s);

	float G3 = 1.0/6.0; // Very nice and simple unskew factor, too
	float t = (i+j+k)*G3;
	float X0 = i-t; // Unskew the cell origin back to (x,y,z) space
	float Y0 = j-t;
	float Z0 = k-t;
	float x0 = xin-X0; // The x,y,z distances from the cell origin
	float y0 = yin-Y0;
	float z0 = zin-Z0;

	unsigned int i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
	unsigned int i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords
	if(x0>=y0) {
	if(y0>=z0)
	{ i1=1; j1=0; k1=0; i2=1; j2=1; k2=0; } // X Y Z order
	else if(x0>=z0) { i1=1; j1=0; k1=0; i2=1; j2=0; k2=1; } // X Z Y order
	else { i1=0; j1=0; k1=1; i2=1; j2=0; k2=1; } // Z X Y order
	}
	else { // x0<y0
	if(y0<z0) { i1=0; j1=0; k1=1; i2=0; j2=1; k2=1; } // Z Y X order
	else if(x0<z0) { i1=0; j1=1; k1=0; i2=0; j2=1; k2=1; } // Y Z X order
	else { i1=0; j1=1; k1=0; i2=1; j2=1; k2=0; } // Y X Z order
	}
	float x1 = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
	float y1 = y0 - j1 + G3;
	float z1 = z0 - k1 + G3;
	float x2 = x0 - i2 + 2.0*G3; // Offsets for third corner in (x,y,z) coords
	float y2 = y0 - j2 + 2.0*G3;
	float z2 = z0 - k2 + 2.0*G3;
	float x3 = x0 - 1.0 + 3.0*G3; // Offsets for last corner in (x,y,z) coords
	float y3 = y0 - 1.0 + 3.0*G3;
	float z3 = z0 - 1.0 + 3.0*G3;
	// Work out the hashed gradient indices of the four simplex corners
	unsigned int ii = i & 255;
	unsigned int jj = j & 255;
	unsigned int kk = k & 255;

	unsigned int gi0 = index(
		ii,
		jj,
		kk);
	unsigned int gi1 = index(
		ii+i1,
		jj+j1,
		kk+k1);
	unsigned int gi2 = index(
		ii+i2,
		jj+j2,
		kk+k2);
	unsigned int gi3 = index(
		ii+1,
		jj+1,
		kk+1);

	/*
	std::cout <<
		gi0 << "\t" <<
		gi1 << "\t" <<
		gi2 << "\t" <<
		gi3 << std::endl;
	*/
	// Calculate the contribution from the four corners
	float t0 = 0.6 - x0*x0 - y0*y0 - z0*z0;
	if(t0<0) n0 = 0.0;
	else {
	t0 *= t0;
	n0 = t0 * t0 * dot(gradients_.at(gi0), vec3(x0, y0, z0));
	}
	float t1 = 0.6 - x1*x1 - y1*y1 - z1*z1;
	if(t1<0) n1 = 0.0;
	else {
	t1 *= t1;
	n1 = t1 * t1 * dot(gradients_.at(gi1), vec3(x1, y1, z1));
	}
	float t2 = 0.6 - x2*x2 - y2*y2 - z2*z2;
	if(t2<0) n2 = 0.0;
	else {
	t2 *= t2;
	n2 = t2 * t2 * dot(gradients_.at(gi2), vec3(x2, y2, z2));
	}
	float t3 = 0.6 - x3*x3 - y3*y3 - z3*z3;
	if(t3<0) n3 = 0.0;
	else {
	t3 *= t3;
	n3 = t3 * t3 * dot(gradients_.at(gi3), vec3(x3, y3, z3));
	}
	// Add contributions from each corner to get the final noise value.
	// The result is scaled to stay just inside [-1,1]
	return 32.0*(n0 + n1 + n2 + n3);
}
