#ifndef SGEVOL_NOISE_VOLUME_HPP_INCLUDED
#define SGEVOL_NOISE_VOLUME_HPP_INCLUDED

#include <fcppt/string.hpp>
#include <sge/image3d/l8.hpp>
#include <sge/image3d/view/const_object.hpp>
#include <sge/image3d/view/object.hpp>
#include <sge/image/store.hpp>

namespace sgevol
{

class noise_volume
{
public:
	typedef sge::image3d::l8 store;
	typedef store::view_type v;
	typedef fcppt::math::vector::static_< float, 3 >::type vec3;

	explicit
	noise_volume(
		std::size_t const);

	sge::image3d::view::const_object const
	const_view() const;

	sge::image3d::view::object
	view();

	v::dim::value_type
	dimension();

	v
	store_view();

private:
	void
	calculate();

	v::dim::value_type dimension_;
	store store_;
	v view_;
};

}
#endif
