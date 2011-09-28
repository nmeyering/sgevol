#ifndef SGEVOL_MODEL_VERTEX_FORMAT_HPP_INCLUDED
#define SGEVOL_MODEL_VERTEX_FORMAT_HPP_INCLUDED

#include <sge/model/obj/vb_converter/convert.hpp>
#include <sgevol/cloud_sphere/vf.hpp>

#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/map/map10.hpp>
#include <boost/mpl/pair.hpp>
#include <fcppt/mpl/index_of.hpp>
#include <fcppt/config/external_end.hpp>

namespace sgevol
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
