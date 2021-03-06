#include <sgevollib/media_path.hpp>
#include <sgevollib/cloud_sphere/model_format.hpp>
#include <sgevollib/cloud_sphere/object.hpp>
#include <sgevollib/cloud_sphere/vf.hpp>
#include <sge/camera/base.hpp>
#include <sge/camera/coordinate_system/forward.hpp>
#include <sge/camera/coordinate_system/object.hpp>
#include <sge/camera/coordinate_system/position.hpp>
#include <sge/camera/coordinate_system/right.hpp>
#include <sge/camera/coordinate_system/up.hpp>
#include <sge/camera/matrix_conversion/world.hpp>
#include <sge/camera/matrix_conversion/world_projection.hpp>
#include <sge/image3d/view/const_object.hpp>
#include <sge/model/obj/create.hpp>
#include <sge/model/obj/instance.hpp>
#include <sge/model/obj/vb_converter/convert.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/first_vertex.hpp>
#include <sge/renderer/lock_mode.hpp>
#include <sge/renderer/matrix4.hpp>
#include <sge/renderer/nonindexed_primitive_type.hpp>
#include <sge/renderer/resource_flags.hpp>
#include <sge/renderer/resource_flags_field.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scalar.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/scoped_vertex_lock.hpp>
#include <sge/renderer/size_type.hpp>
#include <sge/renderer/vector3.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vertex_count.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/renderer/glsl/uniform/int_type.hpp>
#include <sge/renderer/state/cull_mode.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/texture/address_mode.hpp>
#include <sge/renderer/texture/address_mode3.hpp>
#include <sge/renderer/texture/create_volume_from_view.hpp>
#include <sge/renderer/texture/planar_shared_ptr.hpp>
#include <sge/renderer/texture/set_address_mode3.hpp>
#include <sge/renderer/texture/stage.hpp>
#include <sge/renderer/texture/volume.hpp>
#include <sge/renderer/texture/volume_shared_ptr.hpp>
#include <sge/renderer/texture/mipmap/all_levels.hpp>
#include <sge/renderer/texture/mipmap/auto_generate.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/shader/activation_method.hpp>
#include <sge/shader/activation_method_field.hpp>
#include <sge/shader/matrix.hpp>
#include <sge/shader/matrix_flags.hpp>
#include <sge/shader/object.hpp>
#include <sge/shader/object_parameters.hpp>
#include <sge/shader/sampler.hpp>
#include <sge/shader/sampler_sequence.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/variable.hpp>
#include <sge/shader/variable_sequence.hpp>
#include <sge/shader/variable_type.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <sge/timer/reset_when_expired.hpp>
#include <fcppt/format.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/math/pi.hpp>
#include <fcppt/math/twopi.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/path.hpp>
#include <fcppt/config/external_end.hpp>


sgevollib::cloud_sphere::object::object(
	sge::renderer::device &_renderer,
	sge::model::obj::instance &_model,
	boost::filesystem::path const &_vertex_shader_file,
	boost::filesystem::path const &_fragment_shader_file,
	sge::camera::base &_cam,
	sge::renderer::scalar _radius,
	sge::renderer::scalar _opacity,
	sge::renderer::glsl::uniform::int_type _skip,
	sge::image3d::view::const_object const &_tex,
	sge::image3d::view::const_object const &_noise,
	sge::renderer::texture::planar_shared_ptr &_clouds)
:
renderer_(
	_renderer),
model_(
	_model),
vd_(
	renderer_.create_vertex_declaration(
		sge::renderer::vf::dynamic::make_format<vf::format>())),
vb_(
	sge::model::obj::vb_converter::convert<vf::part,model_format>(
		renderer_,
		*vd_,
		sge::renderer::resource_flags_field(
			sge::renderer::resource_flags::readable),
		model_)),
cam_(
	_cam),
opacity_(
	_opacity),
skip_(
	_skip),
tex_(
	_tex),
noise_(
	_noise),
clouds_(
	_clouds),
shader_(
	sge::shader::object_parameters(
		renderer_,
		*vd_,
		sge::shader::vf_to_string<sgevollib::cloud_sphere::vf::format>(),
		fcppt::assign::make_container<sge::shader::variable_sequence>
			(sge::shader::variable(
				"mvp",
				// Typ (uniform oder const_ für Konstanten)
				sge::shader::variable_type::uniform,
				// Wir nehmen wir eine leere Matrix, wir setzen die jedes Frame neu mit der Kamera
				sge::shader::matrix(
					sge::renderer::matrix4::identity(),
					sge::shader::matrix_flags::projection)))
			(sge::shader::variable(
				"mv",
				sge::shader::variable_type::uniform,
				sge::shader::matrix(
					sge::renderer::matrix4::identity(),
					sge::shader::matrix_flags::none)))
			(sge::shader::variable(
				"radius",
				sge::shader::variable_type::uniform,
				_radius))
			(sge::shader::variable(
				"opacity",
				sge::shader::variable_type::uniform,
				_opacity))
			(sge::shader::variable(
				"offset",
				sge::shader::variable_type::uniform,
				static_cast<sge::renderer::scalar>(
					0.f)))
			(sge::shader::variable(
				"skip",
				sge::shader::variable_type::uniform,
				static_cast<sge::renderer::glsl::uniform::int_type>(
					0)))
			(sge::shader::variable(
				"light",
				sge::shader::variable_type::uniform,
				static_cast<sge::renderer::scalar>(
					0.f)))
			(sge::shader::variable(
				"camera",
				sge::shader::variable_type::uniform,
				sge::renderer::vector3())),
		// sampler (sprich Texturen), die wir im Shader brauchen (ebenfalls in $$$HEADER$$$ drin)
		fcppt::assign::make_container<sge::shader::sampler_sequence>
			(sge::shader::sampler(
				"tex", sge::renderer::texture::volume_shared_ptr()))
			(sge::shader::sampler(
				"noise", sge::renderer::texture::volume_shared_ptr()))
			(sge::shader::sampler(
				"clouds", clouds_))
		)
				.vertex_shader(
					_vertex_shader_file)
				.fragment_shader(
					sgevollib::media_path()
					/ FCPPT_TEXT("shaders")
					/ FCPPT_TEXT("fragment")
					/ FCPPT_TEXT("simplex_noise.glsl"))
				.fragment_shader(
					sgevollib::media_path()
					/ FCPPT_TEXT("shaders")
					/ FCPPT_TEXT("fragment")
					/ FCPPT_TEXT("bounding_tests.glsl"))
				.fragment_shader(
					_fragment_shader_file))
{

	sge::renderer::texture::volume_shared_ptr shadertex(
		sge::renderer::texture::create_volume_from_view(
			renderer_,
			tex_,
			sge::renderer::texture::mipmap::off(),
			sge::renderer::resource_flags::none));

	shader_.update_texture(
		"tex",
		shadertex);

	shadertex = sge::renderer::texture::volume_shared_ptr(
		sge::renderer::texture::create_volume_from_view(
			renderer_,
			noise_,
			sge::renderer::texture::mipmap::off(),
			sge::renderer::resource_flags::none));

	shader_.update_texture(
		"noise",
		shadertex);
}

sgevollib::cloud_sphere::object::~object()
{
}

void
sgevollib::cloud_sphere::object::render(float offset)
{
	sge::renderer::texture::set_address_mode3(
		renderer_,
		sge::renderer::texture::stage(0u),
		sge::renderer::texture::address_mode3(
			sge::renderer::texture::address_mode::clamp));

	sge::shader::scoped scoped_shader(
		shader_,
		sge::shader::activation_method_field(
			sge::shader::activation_method::textures));

	shader_.update_uniform(
		"mvp",
		sge::shader::matrix(
			sge::camera::matrix_conversion::world_projection(
				cam_.coordinate_system(),
				cam_.projection_matrix()),
			sge::shader::matrix_flags::projection));

	shader_.update_uniform(
		"camera",
		-cam_.coordinate_system().position().get());

	shader_.update_uniform(
		"opacity",
		opacity_);

	shader_.update_uniform(
		"offset",
		offset);

	shader_.update_uniform(
		"skip",
		skip_);

	shader_.update_uniform(
		"mv",
		sge::shader::matrix(
			sge::camera::matrix_conversion::world(
				cam_.coordinate_system()),
			sge::shader::matrix_flags::none));

	// Vertexbuffer aktivieren. Muss man machen
	sge::renderer::scoped_vertex_declaration const decl_context(
		renderer_,
		*vd_);

	sge::renderer::scoped_vertex_buffer const vb_context(
		renderer_,
		*vb_);

	// Rendern (copypaste)
	renderer_.render_nonindexed(
		sge::renderer::first_vertex(0u),
		sge::renderer::vertex_count(vb_->size()),
		sge::renderer::nonindexed_primitive_type::triangle);

	/*
	if(
			std::abs( cam.gizmo().position().get().x() ) >= 1.0f ||
			std::abs( cam.gizmo().position().get().y() ) >= 1.0f ||
			std::abs( cam.gizmo().position().get().z() ) >= 1.0f
		)
	{
		rend.state(
			sge::renderer::state::list(
				sge::renderer::state::cull_mode::counter_clockwise
			));
	}
	else
	{
		rend.state(
			sge::renderer::state::list(
				sge::renderer::state::cull_mode::clockwise
			));
	}
	*/
	renderer_.state(
		sge::renderer::state::list(
			sge::renderer::state::cull_mode::clockwise));

}
