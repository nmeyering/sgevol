#include <sge/camera/object.hpp>
#include <sge/image3d/view/const_object.hpp>
#include <sge/renderer/texture/address_mode.hpp>
#include <sge/renderer/texture/address_mode3.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/first_vertex.hpp>
#include <sge/renderer/lock_mode.hpp>
#include <sge/renderer/nonindexed_primitive_type.hpp>
#include <sge/renderer/resource_flags.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/scoped_vertex_lock.hpp>
#include <sge/renderer/size_type.hpp>
#include <sge/renderer/state/cull_mode.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/vertex_count.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/renderer/vf/dynamic/part_index.hpp>
#include <sge/shader/activation_method.hpp>
#include <sge/shader/activation_method_field.hpp>
#include <sge/shader/matrix.hpp>
#include <sge/shader/matrix_flags.hpp>
#include <sge/shader/object.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/timer/reset_when_expired.hpp>
#include <sge/model/obj/create.hpp>
#include <sge/model/obj/loader.hpp>
#include <sge/model/obj/vb_converter/convert.hpp>
#include <sge/renderer/matrix4.hpp>
#include <sge/renderer/texture/planar_ptr.hpp>
#include <sge/shader/object_parameters.hpp>
#include <sge/shader/sampler.hpp>
#include <sge/shader/sampler_sequence.hpp>
#include <sge/shader/variable.hpp>
#include <sge/shader/variable_sequence.hpp>
#include <sge/shader/variable_type.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/filesystem/path.hpp>
#include <fcppt/format.hpp>
#include <fcppt/math/pi.hpp>
#include <fcppt/math/twopi.hpp>
#include <sgevol/media_path.hpp>
#include <sgevol/model/vf.hpp>
#include <sgevol/model/object.hpp>

sgevol::model::object::object(
	sge::renderer::device &_renderer,
	fcppt::filesystem::path const &_model_file,
	fcppt::filesystem::path const &_vertex_shader_file,
	fcppt::filesystem::path const &_fragment_shader_file,
	sge::renderer::texture::planar_ptr _tex,
	sge::camera::object &_cam)
:
renderer_(
	_renderer),
model_loader_(
	sge::model::obj::create()),
model_(
	model_loader_->load(
		_model_file)),
vd_(
	renderer_.create_vertex_declaration(
		sge::renderer::vf::dynamic::make_format<vf::format>())),
vb_(
	sge::model::obj::vb_converter::convert<vf::part,vertex_format>(
		renderer_,
		*vd_,
		sge::renderer::resource_flags::readable,
		*model_)),
cam_(
	_cam),
tex_(
	_tex),
shader_(
	sge::shader::object_parameters(
		renderer_,
		*vd_,
		_vertex_shader_file,
		_fragment_shader_file,
		sge::shader::vf_to_string<sgevol::model::vf::format>(),
		fcppt::assign::make_container<sge::shader::variable_sequence>
			(sge::shader::variable(
				"mvp",
				sge::shader::variable_type::uniform,
				sge::shader::matrix(
					sge::renderer::matrix4::identity(),
					sge::shader::matrix_flags::projection)))
			(sge::shader::variable(
				"mv",
				sge::shader::variable_type::uniform,
				sge::shader::matrix(
					sge::renderer::matrix4::identity(),
					sge::shader::matrix_flags::none))),
		fcppt::assign::make_container<sge::shader::sampler_sequence>
			(sge::shader::sampler(
				"tex", tex_))
				))
{
}

sgevol::model::object::~object()
{
}

void
sgevol::model::object::render()
{
	sge::shader::scoped scoped_shader(
		shader_,
		sge::shader::activation_method_field(
			sge::shader::activation_method::textures));

	sge::renderer::scoped_vertex_declaration const decl_context(
		renderer_,
		*vd_);

	sge::renderer::scoped_vertex_buffer const vb_context(
		renderer_,
		*vb_);

	renderer_.render_nonindexed(
		sge::renderer::first_vertex(0),
		sge::renderer::vertex_count(vb_->size()),
		sge::renderer::nonindexed_primitive_type::triangle);

	renderer_.state(
		sge::renderer::state::list(
			sge::renderer::state::cull_mode::clockwise));

	// mvp updaten
	shader_.update_uniform(
		"mvp",
		sge::shader::matrix(
		cam_.mvp(),
		sge::shader::matrix_flags::projection));

	shader_.update_uniform(
		"mv",
		sge::shader::matrix(
		cam_.world(),
		sge::shader::matrix_flags::none));
}
