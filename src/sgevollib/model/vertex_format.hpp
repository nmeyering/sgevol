#ifndef SGEVOLLIB_MODEL_VERTEX_FORMAT_HPP_INCLUDED
#define SGEVOLLIB_MODEL_VERTEX_FORMAT_HPP_INCLUDED

#include <sgevollib/model/vf.hpp>
#include <sge/model/obj/vb_converter/convert.hpp>
#include <fcppt/mpl/index_of.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/map/map10.hpp>
#include <fcppt/config/external_end.hpp>


namespace sgevollib
{
namespace model
{
typedef
boost::mpl::map3
<
	boost::mpl::pair
	<
		sge::model::obj::vb_converter::roles::position,
		fcppt::mpl::index_of
		<
			vf::part::elements,
			vf::position
		>
	>,
	boost::mpl::pair
	<
		sge::model::obj::vb_converter::roles::texcoord,
		fcppt::mpl::index_of
		<
			vf::part::elements,
			vf::texcoord
		>
	>,
	boost::mpl::pair
	<
		sge::model::obj::vb_converter::roles::normal,
		fcppt::mpl::index_of
		<
			vf::part::elements,
			vf::normal
		>
	>
>
vertex_format;
}
}
#endif
