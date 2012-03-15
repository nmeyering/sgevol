#include <sgevollib/media_path.hpp>
#include <sgevollib/shadow_volume.hpp>
#include <sgevollib/vf.hpp>
#include <sge/image/const_raw_pointer.hpp>
#include <sge/image/raw_pointer.hpp>
#include <sge/image3d/dim.hpp>
#include <sge/image3d/view/const_object.hpp>
#include <mizuiro/color/operators.hpp>
#include <mizuiro/color/init/trampoline.hpp>
#include <mizuiro/image/view.hpp>
#include <fcppt/type_name.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/math/clamp.hpp>
#include <fcppt/math/dim/static.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/math/vector/static.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <cstddef>
#include <iostream>
#include <fcppt/config/external_end.hpp>


namespace
{

bool
outside(
		sgevollib::texture3d::vec3 const &p,
		sgevollib::shadow_volume::cloud_view::dim::value_type const &dim
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

sgevollib::shadow_volume::cloud_view::dim
dim_3d(
	sgevollib::shadow_volume::vec3 const &p)
{
	typedef sgevollib::shadow_volume::cloud_view cloud_view;
	typedef cloud_view::dim::value_type value_type;

	return cloud_view::dim(
		static_cast<value_type>(p.x()),
		static_cast<value_type>(p.y()),
		static_cast<value_type>(p.z()));
}

}

namespace sgevollib
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
	typedef v::dim::value_type dimtype;
	try
	{
	vec3 const center(
			static_cast< float >( dimension() ) * .5f,
			static_cast< float >( dimension() ) * .5f,
			static_cast< float >( dimension() ) * .5f);
	vec3 const sun(0.f, 1.f, 0.f);
	float const stepsize = static_cast<float>(dimension()) * .01f;
	unsigned const steps = 100;
	float const occlusion_factor = 1.0f;

	vec3 tmp;
	float shadow;

	shadow_volume::cloud_view const cloud(
		volume_.get().get<
			shadow_volume::cloud_view>());

	for (dimtype z = 0; z < dimension(); ++z)
	{
		progress(100.0f * static_cast<float>(z+1) / static_cast<float>(dimension()));
		for (dimtype y = 0; y < dimension(); ++y)
			for (dimtype x = 0; x < dimension(); ++x)
			{
				shadow = 0.f;

				tmp[0] =
					static_cast<float>(x);
				tmp[1] =
					static_cast<float>(y);
				tmp[2] =
					static_cast<float>(z);

				for (unsigned i = 0; i < steps; ++i)
				{
					tmp += - sun * stepsize;

					if (outside(tmp, dimension()))
						break;

					shadow += occlusion_factor *
						static_cast<float>(
							cloud[
								dim_3d(tmp)].get(
									channel_type()));

					if (shadow > 1.f)
						break;
				}
				shadow = fcppt::math::clamp(
					shadow,
					0.f,
					1.f);
				store_view()[ v::dim(x,y,z) ] =
					color_type(
						(mizuiro::color::init::trampoline<channel_type>() %= shadow));
			}
	}
	}
	catch(fcppt::exception const &e)
	{
		fcppt::io::cerr() << e.string() << FCPPT_TEXT("\n");
	}
	catch(std::exception const &e)
	{
		std::cerr << e.what() << FCPPT_TEXT('\n');
	}
}

}
