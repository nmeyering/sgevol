#ifndef SGEVOL_TEXTURE3D_HPP_INCLUDED
#define SGEVOL_TEXTURE3D_HPP_INCLUDED

#include <sge/image3d/rgba8.hpp>
#include <sge/image3d/view/const_object.hpp>
#include <sge/image/store.hpp>
#include "locked_value.hpp"

namespace sgevol
{

class texture3d
{
public:
	typedef sge::image3d::rgba8 store;
	typedef store::view_type v;

	explicit
	texture3d(
		std::size_t const dimension);

	sge::image3d::view::const_object const
	view() const;

	void
	calculate();

	float
	progress();
private:
	v::dim_type::value_type dimension_;
	store store_;
	v view_;
	locked_value<float> progress_;
};

}
#endif
