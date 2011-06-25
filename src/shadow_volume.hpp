#ifndef SGEVOL_SHADOW_VOLUME_HPP_INCLUDED
#define SGEVOL_SHADOW_VOLUME_HPP_INCLUDED

#include "texture3d.hpp"

namespace sgevol
{

class shadow_volume: public texture3d
{
	public:
		typedef
		/*
		sge::image::detail::view::mizuiro_type<
			sge::image::color::gray8_format,boost::mpl::integral_c<
				sge::image::size_type,
				3
			>
		>::type
		*/
		sge::image3d::view::const_element<
			sge::image3d::gray8_format
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
