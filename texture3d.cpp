#include <sge/image/color/any/convert.hpp>
#include <sge/image/color/init.hpp>
#include <sge/image/color/rgba8.hpp>
#include <sge/image/color/rgba8_format.hpp>
#include <sge/image/const_raw_pointer.hpp>
#include <sge/image/raw_pointer.hpp>
#include <sge/image3d/dim.hpp>
#include <sge/image3d/view/format.hpp>
#include <sge/image3d/view/make.hpp>
#include <sge/image3d/view/make_const.hpp>
#include <sge/image3d/view/object.hpp>
#include <sge/image3d/view/optional_pitch.hpp>
#include <sge/image3d/view/to_const.hpp>
#include <mizuiro/image/view.hpp>
#include <fcppt/container/raw_vector.hpp>
#include <fcppt/make_shared_ptr.hpp>
#include <fcppt/math/clamp.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/dim/input.hpp>
#include <fcppt/math/dim/output.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/vector/basic_impl.hpp>
#include <fcppt/math/vector/length.hpp>
#include <fcppt/signal/scoped_connection.hpp>
#include <fcppt/text.hpp>
#include <boost/bind.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <cstddef>
#include <cstdlib>
#include <ostream>
#include <streambuf>
#include <utility>
#include "media_path.hpp"
#include "simplex_noise.hpp"
#include "texture3d.hpp"
#include "vf.hpp"

namespace sgevol
{
texture3d::texture3d(
	v::dim_type::value_type const _dimension
	)
:
	dimension_(
		_dimension
		),
	store_(
		store::dim_type(
			_dimension,
			_dimension,
			_dimension)),
	view_(
		store_.view())
{
	progress_.value(0.0f);
}

sge::image3d::view::const_object const
texture3d::view() const
{
	// Das hier macht aus einer Sammlung von Bytes eine Struktur, nämlich ein
	// mizuiro::view. Das braucht der Renderer, um eine Textur draus zu
	// generieren. Sollte alles selbsterklärend sein (ausnahmsweise)
	return 
		sge::image3d::view::to_const( view_ );
}

float 
texture3d::progress()
{
	return progress_.value();
}

void
texture3d::calculate()
{
	double red = 1.0;
	double green = 1.0;
	double blue = 1.0;
	double alpha = 0.0;
	typedef fcppt::math::vector::static_< float, 3 >::type vec3;
	typedef v::dim_type::value_type dimtype;
	vec3 center(
			static_cast< float >( dimension_ ) * .5f,
			static_cast< float >( dimension_ ) * .5f,
			static_cast< float >( dimension_ ) * .5f);
	sgevol::simplex_noise<float,3> noise( 128, 256 );
	vec3 tmp = vec3(42.f,13.f,37.f);
	for (dimtype z = 0; z < dimension_; ++z)
	{
		progress_.value( 100.0f * static_cast<float>(z+1) / static_cast<float>(dimension_) );
		for (dimtype y = 0; y < dimension_; ++y)
			for (dimtype x = 0; x < dimension_; ++x)
			{
				tmp[0] = 
					static_cast< float >( x );
				tmp[1] = 
					static_cast< float >( y );
				tmp[2] = 
					static_cast< float >( z );
				
				float const scale = 0.5f;


				#if 1
				alpha = 
					fcppt::math::clamp(
						0.0625f * noise.sample( scale * 0.2f * tmp ) +
						0.125f * noise.sample( scale * 0.10f * tmp ) +
						0.25f * noise.sample( scale * 0.05f * tmp ) +
						0.5f * noise.sample( scale * 0.025f * tmp )
						,0.f
						,1.f);
				//sphere
				alpha *= 
					fcppt::math::clamp(
						1.0f -
						(0.5f + fcppt::math::vector::length( tmp - center )) /
						(0.5f * static_cast<float>(dimension_)),
						/*
						(
							fcppt::math::vector::length(tmp - center) < 
								0.5f * static_cast<float>(dimension_) ?
								1.f :
								0.f ),
						*/
						0.f,
						1.f
					);
				#else
				alpha = fcppt::math::clamp(
					1.0f - 
					8.f * tmp[1] / static_cast<float>(dimension_),
					0.f,
					1.f
				);

				alpha += fcppt::math::clamp(
					1.0f - 
					fcppt::math::vector::length(tmp - center) / 
					(0.3f * static_cast<float>(dimension_)),
					0.f,
					1.f
				);
				#endif
					
				view_[ v::dim_type(x,y,z) ] = 
					sge::image::color::rgba8(
						(sge::image::color::init::red %= red)
						(sge::image::color::init::green %= green)
						(sge::image::color::init::blue %= blue)
						(sge::image::color::init::alpha %= alpha));
			}
	}
}

}
