#ifndef SGEVOLLIB_CLOUD_SPHERE_MODEL_FORMAT_HPP_INCLUDED
#define SGEVOLLIB_CLOUD_SPHERE_MODEL_FORMAT_HPP_INCLUDED

#include <sgevollib/cloud_sphere/vf.hpp>
#include <sge/model/obj/vb_converter/convert.hpp>
#include <fcppt/mpl/index_of.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/map/map10.hpp>
#include <fcppt/config/external_end.hpp>


namespace sgevollib
{
namespace cloud_sphere
{
typedef
boost::mpl::map1
<
	boost::mpl::pair
	<
		sge::model::obj::vb_converter::roles::position,
		fcppt::mpl::index_of
		<
			vf::part::elements,
			vf::position
		>
	>
>
model_format;
}
}
#endif
