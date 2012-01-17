#include <sgevollib/noise_volume.hpp>
#include <sgevollib/simplex_noise.hpp>
#include <sge/image/color/init.hpp>
#include <sge/image3d/view/const_object.hpp>
#include <sge/image3d/view/object.hpp>
#include <fcppt/math/clamp.hpp>


sgevollib::noise_volume::noise_volume(
	v::dim::value_type const _dimension
	)
:
	dimension_(
		_dimension),
	store_(
		store::dim(
			_dimension,
			_dimension,
			_dimension)),
	view_(
		store_.view())
{
	calculate();
}

sge::image3d::view::const_object const
sgevollib::noise_volume::const_view() const
{
	return
		sge::image3d::view::const_object( store_.wrapped_view() );
}

sge::image3d::view::object
sgevollib::noise_volume::view()
{
	return
		sge::image3d::view::object( store_.wrapped_view() );
}

sgevollib::noise_volume::v::dim::value_type
sgevollib::noise_volume::dimension()
{
	return dimension_;
}

sgevollib::noise_volume::v
sgevollib::noise_volume::store_view()
{
	return view_;
}

void
sgevollib::noise_volume::calculate()
{
	double alpha = 1.0;
	typedef v::dim::value_type dimtype;
	vec3 center(
			static_cast< float >( dimension_ ) * .5f,
			static_cast< float >( dimension_ ) * .5f,
			static_cast< float >( dimension_ ) * .5f);
	sgevollib::simplex_noise<float,3> noise( dimension_, 256 );
	vec3 tmp = vec3(42.f,13.f,37.f);
	for (dimtype z = 0; z < dimension_; ++z)
		for (dimtype y = 0; y < dimension_; ++y)
			for (dimtype x = 0; x < dimension_; ++x)
			{
				tmp[0] =
					static_cast< float >( x );
				tmp[1] =
					static_cast< float >( y );
				tmp[2] =
					static_cast< float >( z );

				alpha =
					fcppt::math::clamp(
						noise.sample( tmp )
						,0.f
						,1.f);

				view_[ v::dim(x,y,z) ] =
					color_type(
						(mizuiro::color::init::trampoline<channel_type>() %= alpha));
			}
}
