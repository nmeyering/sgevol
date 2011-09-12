#ifndef SGEVOL_TEXTURE3D_HPP_INCLUDED
#define SGEVOL_TEXTURE3D_HPP_INCLUDED

#include <fcppt/string.hpp>
#include <sge/image3d/l8.hpp>
#include <sge/image3d/view/const_object.hpp>
#include <sge/image3d/view/object.hpp>
#include <sge/image/store.hpp>
#include "locked_value.hpp"

namespace sgevol
{

class texture3d
{
public:
	typedef sge::image3d::l8 store;
	typedef store::view_type v;
	typedef fcppt::math::vector::static_< float, 3 >::type vec3;

	explicit
	texture3d(
		std::size_t const);

	explicit
	texture3d(
		std::size_t const,
		fcppt::filesystem::path const &);

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
	calculate();

	void
	save(
		fcppt::filesystem::path const &);

	void
	load(
		fcppt::filesystem::path const &);

private:
	v::dim::value_type dimension_;
	store store_;
	v view_;
	locked_value<float> progress_;
};

}
#endif
