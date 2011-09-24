#ifndef SGEVOL_MODEL_VF_HPP_INCLUDED
#define SGEVOL_MODEL_VF_HPP_INCLUDED

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
namespace model
{
namespace vf
{

namespace tags
{
SGE_RENDERER_VF_MAKE_UNSPECIFIED_TAG(position);
SGE_RENDERER_VF_MAKE_UNSPECIFIED_TAG(normal);
SGE_RENDERER_VF_MAKE_UNSPECIFIED_TAG(texcoord);
}

typedef
sge::renderer::vf::unspecified
<
	sge::renderer::vf::vector
	<
		sge::renderer::scalar,
		3
	>,
	tags::normal
>
normal;

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
		2
	>,
	tags::texcoord
>
texcoord;

typedef
sge::renderer::vf::part
<
	boost::mpl::vector3
	<
		position,
		texcoord,
		normal
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
