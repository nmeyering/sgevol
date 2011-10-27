#include <sge/image/color/color.hpp>
#include <sge/image/const_raw_pointer.hpp>
#include <sge/image/raw_pointer.hpp>
#include <sge/image3d/dim.hpp>
#include <sge/image3d/view/view.hpp>
#include <mizuiro/image/view.hpp>
#include <fcppt/container/raw_vector.hpp>
#include <fcppt/io/cofstream.hpp>
#include <fcppt/io/cifstream.hpp>
#include <fcppt/filesystem/file_size.hpp>
#include <fcppt/filesystem/path.hpp>
#include <fcppt/format.hpp>
#include <fcppt/make_shared_ptr.hpp>
#include <fcppt/math/clamp.hpp>
#include <fcppt/math/dim/dim.hpp>
#include <fcppt/math/vector/vector.hpp>
#include <fcppt/signal/scoped_connection.hpp>
#include <fcppt/text.hpp>
#include <boost/bind.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <cstddef>
#include <cstdlib>
#include <ostream>
#include <streambuf>
#include <utility>
#include <sgevol/media_path.hpp>
#include <sgevol/simplex_noise.hpp>
#include <sgevol/texture3d.hpp>
#include <sgevol/vf.hpp>

namespace sgevol
{
texture3d::texture3d(
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
	progress(0.0f);
}

texture3d::texture3d(
	v::dim::value_type const _dimension,
	fcppt::filesystem::path const &_filename)
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
	progress(0.0f);
	load(_filename);
}

void texture3d::load(
		fcppt::filesystem::path const &_filename)
{
	typedef
	fcppt::io::cifstream::pos_type
	pos_type;

	fcppt::io::cifstream file(
		_filename);

	pos_type size =
		sge::image3d::view::size(
				view()
			).content() *
				sge::image::color::format_stride(
					sge::image3d::view::format(
						view()));

	if (!file.is_open())
		throw fcppt::exception(
			FCPPT_TEXT("Failed to open texture file!"));

	pos_type chunk_size =
			static_cast<int>(size) / 100;

	char *buffer = reinterpret_cast<
				char *
			>(
				sge::image3d::view::data(
					view()));

	for (unsigned chunk = 0; chunk < 100; ++chunk)
	{
		progress(
			static_cast<float>(
				chunk) / 100.f);
		file.read(
			buffer,
			chunk_size);
		buffer += chunk_size;
	}

	unsigned rest = size % 100u;
	file.read(buffer,rest);
	progress(100.f);

	pos_type read_size = file.tellg();

	if (read_size < size)
	{
		throw fcppt::exception(
			(fcppt::format(
				FCPPT_TEXT("Texture file size %1%")
				FCPPT_TEXT(" didn't match expected size %2%."))
				%	read_size
				%	size
			).str());
	}

	file.ignore();

	if (!file.eof())
	{
		throw fcppt::exception(
			(fcppt::format(
				FCPPT_TEXT("Texture file too big,")
				FCPPT_TEXT(" expected size %1%."))
				%	size
			).str());
	}
	file.close();
}

sge::image3d::view::const_object const
texture3d::const_view() const
{
	return
		sge::image3d::view::const_object( store_.wrapped_view() );
}

sge::image3d::view::object
texture3d::view()
{
	return
		sge::image3d::view::object( store_.wrapped_view() );
}

texture3d::v::dim::value_type
texture3d::dimension()
{
	return dimension_;
}

texture3d::v
texture3d::store_view()
{
	return view_;
}

float
texture3d::progress()
{
	return progress_.value();
}

void
texture3d::progress(
	float const _progress)
{
	progress_.value(
		_progress);
}

void
texture3d::save(
	fcppt::filesystem::path const &_filename)
{
	fcppt::io::cofstream file(
		_filename);

	sge::image3d::dim::value_type size = sge::image3d::view::size(
			const_view()
		).content() *
		sge::image::color::format_stride(
			sge::image3d::view::format(
				const_view()));

	file.write(
		reinterpret_cast<
			char const *
		>(
			sge::image3d::view::data(
				const_view())),
		size);

	file.close();
}

void
texture3d::calculate()
{
	double alpha = 1.0;
	typedef v::dim::value_type dimtype;
	vec3 center(
			static_cast< float >( dimension_ ) * .5f,
			static_cast< float >( dimension_ ) * .5f,
			static_cast< float >( dimension_ ) * .5f);
	sgevol::simplex_noise<float,3> noise( 128, 256 );
	vec3 tmp = vec3(42.f,13.f,37.f);
	float const scale = (1.f/256.0) * static_cast<float>(dimension_);
	for (dimtype z = 0; z < dimension_; ++z)
	{
		progress( 100.0f * static_cast<float>(z+1) / static_cast<float>(dimension_) );
		for (dimtype y = 0; y < dimension_; ++y)
			for (dimtype x = 0; x < dimension_; ++x)
			{
				tmp[0] =
					static_cast< float >( x );
				tmp[1] =
					static_cast< float >( y );
				tmp[2] =
					static_cast< float >( z );

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
				#if 1
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
				#endif
				#else
				alpha = fcppt::math::clamp(
					1.0f -
					8.f * tmp[1] / static_cast<float>(dimension_),
					0.f,
					1.f
				);

				alpha +=
					(fcppt::math::vector::length(tmp -
						1.5f * center) /
					(0.2f * static_cast<float>(dimension_))) < 1.f ?
						1.f:
						0.f;

				alpha +=
					(fcppt::math::vector::length(tmp -
						0.5f * center) /
					(0.2f * static_cast<float>(dimension_))) < 1.f ?
						1.f:
						0.f;
				#endif

				view_[ v::dim(x,y,z) ] =
					sge::image::color::l8(
						(sge::image::color::init::luminance() %= alpha));
			}
	}
}

}
