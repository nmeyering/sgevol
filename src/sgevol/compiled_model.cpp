#include <fcppt/cref.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/scoped_ptr.hpp>
#include <sge/model/obj/instance.hpp>
#include <sge/model/obj/vb_converter/convert.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/first_vertex.hpp>
#include <sge/renderer/nonindexed_primitive_type.hpp>
#include <sge/renderer/resource_flags.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/stage.hpp>
#include <sge/renderer/texture/planar_ptr.hpp>
#include <sge/renderer/texture/scoped.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/renderer/vertex_count.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include "vf.hpp"
#include "compiled_model.hpp"

namespace
{

typedef
boost::mpl::insert
<
	texcoord_format,
	boost::mpl::pair
	<
		sge::model::obj::vb_converter::roles::normal,
		fcppt::mpl::index_of
		<
			vf::format_part::elements,
			vf::normal
		>
	>
>::type
texcoord_normal_format;

sge::renderer::vertex_buffer_ptr const
choose_format_and_convert(
	sge::renderer::device &_renderer,
	sge::renderer::vertex_declaration const &_vd,
	sge::model::obj::instance const &_model,
	sge::renderer::texture::planar_ptr const _texture)
{
		// require texture and normals
		return
			sge::model::obj::vb_converter::convert<sgevol::vf::part,texcoord_normal_format>(
				_renderer,
				_vd,
				sge::renderer::resource_flags::readable,
				_model);
}
}

namespace sgevol
{
compiled_model::compiled_model(
	sge::model::obj::instance const &_model,
	sge::renderer::device &_renderer,
	sge::renderer::vertex_declaration const &_vd,
	sge::renderer::texture::planar_ptr const _texture)
:
	vd_(
		_vd),
	vb_(
		choose_format_and_convert(
			_renderer,
			_vd,
			_model,
			_texture)),
	texture_(
		_texture),
	renderer_(
		_renderer)
{
}

void
compiled_model::render()
{
	fcppt::scoped_ptr<sge::renderer::texture::scoped> scoped_texture;

	if(texture_)
		scoped_texture.take(
			fcppt::make_unique_ptr<sge::renderer::texture::scoped>(
				fcppt::ref(
					renderer_),
				fcppt::cref(
					*texture_),
				sge::renderer::stage(
					0)));

	sge::renderer::scoped_vertex_declaration scoped_vd(
		renderer_,
		vd_);

	sge::renderer::scoped_vertex_buffer scoped_vb(
		renderer_,
		*vb_);

	renderer_.render_nonindexed(
		sge::renderer::first_vertex(
			0),
		sge::renderer::vertex_count(
			vb_->size()),
		sge::renderer::nonindexed_primitive_type::triangle);
}

compiled_model::~compiled_model()
{
}

}
