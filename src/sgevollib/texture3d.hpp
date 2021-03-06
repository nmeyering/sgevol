#ifndef SGEVOLLIB_TEXTURE3D_HPP_INCLUDED
#define SGEVOLLIB_TEXTURE3D_HPP_INCLUDED

#include <sgevollib/locked_value.hpp>
#include <sge/image/store.hpp>
#include <sge/image/color/l8.hpp>
#include <sge/image/color/l8_format.hpp>
#include <sge/image3d/l8.hpp>
#include <sge/image3d/view/const_object.hpp>
#include <sge/image3d/view/object.hpp>
#include <mizuiro/color/channel/luminance.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/string.hpp>
#include <fcppt/math/vector/object_impl.hpp>
#include <fcppt/math/vector/static.hpp>


namespace sgevollib
{
class texture3d
{
FCPPT_NONCOPYABLE(texture3d);
public:
	typedef sge::image3d::l8 store;
	typedef sge::image::color::l8 color_type;
	typedef mizuiro::color::channel::luminance channel_type;
	typedef sge::image::color::l8_format color_format;
	typedef store::view_type v;
	typedef fcppt::math::vector::static_< float, 3 >::type vec3;

	explicit
	texture3d(
		std::size_t const);

	explicit
	texture3d(
		std::size_t const,
		boost::filesystem::path const &);

	sge::image3d::view::const_object const
	const_view() const;

	sge::image3d::view::object
	view();

	v::dim::value_type
	dimension();

	float
	progress();

	void
	progress(
		float const);

	v
	store_view();

	void
	fill();

	void
	fill_spherical();

	void
	save(
		boost::filesystem::path const &);

	void
	load(
		boost::filesystem::path const &);

private:
	v::dim::value_type dimension_;
	store store_;
	v view_;
	locked_value<float> progress_;
};
}
#endif
