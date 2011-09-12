#ifndef SGEVOL_SHADOW_VOLUME_HPP_INCLUDED
#define SGEVOL_SHADOW_VOLUME_HPP_INCLUDED

#include "texture3d.hpp"

namespace sgevol
{

class shadow_volume: public texture3d
{
	public:
		typedef
		sge::image3d::view::const_element<
			sge::image::color::l8_format
		>::type
		cloud_view;

		explicit
		shadow_volume(
			std::size_t const,
			sge::image3d::view::const_object const &);

		void calculate();
	private:
		sge::image3d::view::const_object volume_;
};

}
#endif
