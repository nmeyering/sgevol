#include <sge/image/color/color.hpp>
#include <sge/image/const_raw_pointer.hpp>
#include <sge/image/raw_pointer.hpp>
#include <sge/image3d/dim.hpp>
#include <sge/image3d/view/view.hpp>
#include <mizuiro/image/view.hpp>
#include <mizuiro/color/operators.hpp>
#include <mizuiro/color/channel/luminance.hpp>
#include <fcppt/math/clamp.hpp>
#include <fcppt/math/dim/dim.hpp>
#include <fcppt/math/vector/vector.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <cstddef>
#include "media_path.hpp"
#include "shadow_volume.hpp"
#include "vf.hpp"

namespace
{

bool
outside(
		sgevol::shadow_volume::vec3 const &p,
		sgevol::shadow_volume::cloud_view::dim_type::value_type const &dim
	)
{
	float const fdim = static_cast<float>(dim);
	return
		p.x() < 0.f ||
		p.y() < 0.f ||
		p.z() < 0.f ||
		p.x() > fdim ||
		p.y() > fdim ||
		p.z() > fdim;
}

sgevol::shadow_volume::cloud_view::dim_type
dim_type_3d(
	sgevol::shadow_volume::vec3 const &p)
{
	typedef sgevol::shadow_volume::cloud_view cloud_view;
	typedef cloud_view::dim_type::value_type value_type;

	return cloud_view::dim_type(
		static_cast<value_type>(p.x()),
		static_cast<value_type>(p.y()),
		static_cast<value_type>(p.z()));
}

}

namespace sgevol
{

shadow_volume::shadow_volume(
	std::size_t const _dimension,
	sge::image3d::view::const_object const &_volume)
:
texture3d(
	_dimension),
volume_(
	_volume)
{
}

void
shadow_volume::calculate()
{
	typedef v::dim_type::value_type dimtype;
	vec3 const center(
			static_cast< float >( dimension() ) * .5f,
			static_cast< float >( dimension() ) * .5f,
			static_cast< float >( dimension() ) * .5f);
	vec3 const sun(0.f, 1.f, 0.f);
	float const stepsize = static_cast<float>(dimension()) * .01f;
	unsigned const steps = 100;
	float const occlusion_factor = .1f;

	vec3 tmp;
	float light;

	shadow_volume::cloud_view const cloud(
		volume_.get<
			shadow_volume::cloud_view>());

	for (dimtype z = 0; z < dimension(); ++z)
	{
		progress( 100.0f * static_cast<float>(z+1) / static_cast<float>(dimension()));
		for (dimtype y = 0; y < dimension(); ++y)
			for (dimtype x = 0; x < dimension(); ++x)
			{
				light = 1.f;

				tmp[0] = 
					static_cast< float >( x );
				tmp[1] = 
					static_cast< float >( y );
				tmp[2] = 
					static_cast< float >( z );
				
				for (unsigned i = 0; i < steps; ++i)
				{
					tmp += - sun * stepsize;
					if (outside(tmp))
						break;
					light -= occlusion_factor *
						static_cast<float>(
							cloud[
								dim_type_3d(tmp)].get<
									mizuiro::color::channel::luminance>());
				}
				store_view()[ v::dim_type(x,y,z) ] = 
					sge::image::color::gray8(
						(sge::image::color::init::luminance %= light));
			}
	}
}

}
