#include "perlin3d.hpp"

using sgevol::perlin3d;

float perlin3d::sample(
	vec3 const &foo
)
{
	return 0.0f;
}

perlin3d::perlin3d(
	std::size_t const _dim
)
:
	dim_(
		_dim
		),
	grid_(
		grid_type::dim(
			_dim,
			_dim,
			_dim
		)
	)
{
	//TODO: zufällige Gradienten erzeugen, etc.
	/*
	container_type indices(255);
	// Fülle mit Werten von 0 bis Größe
	boost::iota(indices,static_cast<index_typ>(0));
	boost::random_shuffle(indices);
	*/
}

