#ifndef SGEVOL_STARS_VF_HPP_INCLUDED
#define SGEVOL_STARS_VF_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <sge/renderer/vf/format.hpp>
#include <sge/renderer/vf/make_unspecified_tag.hpp>
#include <sge/renderer/vf/part.hpp>
#include <sge/renderer/vf/unspecified.hpp>
#include <sge/renderer/vf/vector.hpp>
#include <sge/renderer/vf/view.hpp>

namespace sgevol
{
namespace stars
{
namespace vf
{

namespace tags
{
SGE_RENDERER_VF_MAKE_UNSPECIFIED_TAG(position);
SGE_RENDERER_VF_MAKE_UNSPECIFIED_TAG(radius);
}

typedef
sge::renderer::vf::unspecified
<
	sge::renderer::vf::vector
	<
		sge::renderer::scalar,
		3
	>,
	tags::position
>
position;

typedef
sge::renderer::vf::unspecified
<
	sge::renderer::vf::vector
	<
		sge::renderer::scalar,
		1
	>,
	tags::radius
>
radius;

typedef
sge::renderer::vf::part
<
	boost::mpl::vector2
	<
		position,
		radius
	>
>
part;

typedef
sge::renderer::vf::format
<
	boost::mpl::vector1<part>
>
format;


typedef
sge::renderer::vf::view<part>
vertex_view;

}
}
}
#endif
