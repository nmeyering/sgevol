#include <sgevollib/media_path.hpp>
#include <sgevollib/simplex_noise.hpp>
#include <sgevollib/texture3d.hpp>
#include <sgevollib/vf.hpp>
#include <sge/image/const_raw_pointer.hpp>
#include <sge/image/raw_pointer.hpp>
#include <sge/image/color/format_stride.hpp>
#include <sge/image3d/dim.hpp>
#include <sge/image3d/view/const_object.hpp>
#include <sge/image3d/view/data.hpp>
#include <sge/image3d/view/format.hpp>
#include <sge/image3d/view/object.hpp>
#include <sge/image3d/view/size.hpp>
#include <mizuiro/image/view.hpp>
#include <fcppt/format.hpp>
#include <fcppt/make_shared_ptr.hpp>
#include <fcppt/text.hpp>
#include <fcppt/container/raw_vector.hpp>
#include <fcppt/math/clamp.hpp>
#include <fcppt/math/pi.hpp>
#include <fcppt/math/twopi.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/math/vector/length.hpp>
#include <fcppt/signal/scoped_connection.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <ostream>
#include <streambuf>
#include <utility>
#include <fcppt/config/external_end.hpp>


namespace
{
typedef sgevollib::texture3d::vec3 vec3;

float
cloudy_noise(
		vec3 const &_p,
		float const _freq,
		sgevollib::simplex_noise<float,3> &_noise,
		float const _skew = 0.5f)
{
	return
		fcppt::math::clamp(
				(1.f - _skew) + _skew * (
				0.0625f * _noise.sample(_freq * 0.8f * _p) +
				0.125f 	* _noise.sample(_freq * 0.4f * _p) +
				0.25f 	* _noise.sample(_freq * 0.2f * _p) +
				0.5f 		* _noise.sample(_freq * 0.1f * _p)),
			0.f,
			1.f);
}

float
margin_falloff(
		vec3 const &_p,
		float const _dim,
		float const _margin)
{
	return (
		(_p.x() < _margin || _p.x() > _dim - _margin) ||
		(_p.y() < _margin || _p.y() > _dim - _margin) ||
		(_p.z() < _margin || _p.z() > _dim - _margin)) ?
			0.f :
			1.f;
}

float
sphere_falloff(
		vec3 const &_p,
		vec3 const &_center,
		float const _dim,
		float const _sharpness)
{
	float radius =
		fcppt::math::vector::length(
			_p -
			_center
		) * 2.f / _dim;

	return
		fcppt::math::clamp(
			(radius - 0.99f) /
			(_sharpness - 1.f),
			0.0f,
			1.0f);
}

float
checkerboard(
	vec3 const &p,
	float const scale)
{
	return
		(((std::fmod(p.x(),1.f/scale) > (0.5f/scale)) !=
		(std::fmod(p.y(),1.f/scale) > (0.5f/scale))) ||
		((std::fmod(p.x(),1.f/scale) > (0.5f/scale)) !=
		(std::fmod(p.z(),1.f/scale) > (0.5f/scale))))
					? 0.f : 1.f;
}

vec3
to_spherical(
	vec3 const &p)
{
	using fcppt::math::vector::length;
	using std::acos;
	using std::atan2;

	float const pi = fcppt::math::pi<float>();
	float const twopi = fcppt::math::twopi<float>();

	float const r = length(p);
	float const phi = atan2(p.y(), p.x());
	float const theta = acos(p.z() /r);

	return vec3(
			r,
			phi/twopi + .5f,
			theta/pi);
}

vec3
to_cartesian(
	const vec3 &p)
{
	using std::sin;
	using std::cos;

	// phi in [0,2pi[, theta in [0,pi]
	float const r = p.x();
	float const phi = p.y();
	float const theta = p.z();

	return vec3(
			r * cos(phi) * sin(theta),
			r * sin(phi) * sin(theta),
			r * cos(theta));
}

}

namespace sgevollib
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
	boost::filesystem::path const &_filename)
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
		boost::filesystem::path const &_filename)
{
	typedef
	boost::filesystem::ifstream::pos_type
	pos_type;

	boost::filesystem::ifstream file(
		_filename);

	pos_type size =
		static_cast<long>(
		sge::image3d::view::size(
				view()
			).content() *
				sge::image::color::format_stride(
					sge::image3d::view::format(
						view())));

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

	std::streamoff rest = size % 100;
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
	boost::filesystem::path const &_filename)
{
	boost::filesystem::ofstream file(
		_filename);

	std::streamsize size = static_cast<std::streamsize>(sge::image3d::view::size(
			const_view()
		).content() *
		sge::image::color::format_stride(
			sge::image3d::view::format(
				const_view())));

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
texture3d::fill_spherical()
{
	typedef v::dim::value_type dimtype;

	vec3 center(
			static_cast<float>(dimension_) * .5f,
			static_cast<float>(dimension_) * .5f,
			static_cast<float>(dimension_) * .5f);

	sgevollib::simplex_noise<float,3> noise(1024, 256);
	vec3 pos = vec3::null();

	float const dim = static_cast<float>(dimension_);
	float const scale = 256.f / dim;
	float const pi = fcppt::math::pi<float>();
	float const radius = 0.95f;

	float alpha = 0.f;

	for (dimtype z = 0; z < dimension_; ++z)
	{
		progress(
				100.0f * static_cast<float>(z+1) /
				dim);

		for (dimtype y = 0; y < dimension_; ++y)
			for (dimtype x = 0; x < dimension_; ++x)
			{
				vec3 tmp(
					radius + (1.f - radius) * (static_cast<float>(x) / dim),
					2.f * pi * static_cast<float>(y) / dim,
					pi * static_cast<float>(z) / dim);
				tmp = (to_cartesian(tmp) + vec3(1.f,1.f,1.f)) * .5f;
				tmp *= dim;

				alpha = cloudy_noise(tmp, scale, noise, 0.95f);
				// alpha *= fcppt::math::clamp(.95f - (static_cast<float>(x) / dim), 0.f, 1.f);
				alpha *= sphere_falloff(tmp, center, dim, 0.95f);

				view_[ v::dim(x,y,z) ] =
					color_type(
						(mizuiro::color::init::trampoline<channel_type>() %= alpha));
			}
	}
}

void
texture3d::fill()
{
	float alpha = 0.f;
	typedef v::dim::value_type dimtype;
	float const dim = static_cast<float>(dimension_);
	vec3 center(
			dim * .5f,
			dim * .5f,
			dim * .5f);
	sgevollib::simplex_noise<float,3> noise(1024, 256);
	vec3 tmp = vec3::null();
	float const scale = 256.f / dim;
	float const margin = 0.05f * dim;
	for (dimtype z = 0; z < dimension_; ++z)
	{
		progress(100.0f * static_cast<float>(z+1) / dim);
		for (dimtype y = 0; y < dimension_; ++y)
			for (dimtype x = 0; x < dimension_; ++x)
			{
				tmp[0] =
					static_cast<float>(x) + 0.5f;
				tmp[1] =
					static_cast<float>(y) + 0.5f;
				tmp[2] =
					static_cast<float>(z) + 0.5f;

				//alpha = checkerboard(tmp, 4.f/dim);

				alpha = cloudy_noise(tmp, scale, noise, 0.80f);
				alpha *= sphere_falloff(tmp, center, dim, 0.97f);

				view_[ v::dim(x,y,z) ] =
					color_type(
						(mizuiro::color::init::trampoline<channel_type>() %= alpha));
			}
	}
}

}
