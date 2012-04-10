#include <sgevollib/media_path.hpp>
#include <sgevollib/cube/object.hpp>
#include <sgevollib/cube/vf.hpp>
#include <sge/camera/base.hpp>
#include <sge/camera/matrix_conversion/world.hpp>
#include <sge/image3d/view/const_object.hpp>
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
#include <sge/renderer/vector3.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vertex_count.hpp>
#include <sge/renderer/state/cull_mode.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/texture/address_mode.hpp>
#include <sge/renderer/texture/address_mode3.hpp>
#include <sge/renderer/texture/create_volume_from_view.hpp>
#include <sge/renderer/texture/set_address_mode3.hpp>
#include <sge/renderer/texture/mipmap/all_levels.hpp>
#include <sge/renderer/texture/mipmap/auto_generate.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/renderer/texture/volume.hpp>
#include <sge/renderer/vf/iterator.hpp>
#include <sge/renderer/vf/vertex.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/renderer/vf/dynamic/part_index.hpp>
#include <sge/shader/activation_method.hpp>
#include <sge/shader/activation_method_field.hpp>
#include <sge/shader/matrix.hpp>
#include <sge/shader/matrix_flags.hpp>
#include <sge/shader/object.hpp>
#include <sge/shader/object_parameters.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/variable_sequence.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <fcppt/format.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/math/pi.hpp>
#include <fcppt/math/twopi.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/path.hpp>
#include <algorithm>
#include <fcppt/config/external_end.hpp>

#include <sge/renderer/matrix4.hpp>
#include <sge/renderer/scalar.hpp>
#include <sge/renderer/texture/planar_shared_ptr.hpp>
#include <sge/renderer/texture/stage.hpp>
#include <sge/renderer/texture/volume_shared_ptr.hpp>
#include <sge/shader/sampler.hpp>
#include <sge/shader/sampler_sequence.hpp>
#include <sge/shader/variable.hpp>
#include <sge/shader/variable_type.hpp>
#include <sge/camera/coordinate_system/object.hpp>
#include <sge/camera/coordinate_system/position.hpp>

sgevollib::cube::object::object(
	sge::renderer::device &_renderer,
	boost::filesystem::path const &_vertex_shader_file,
	boost::filesystem::path const &_fragment_shader_file,
	sge::camera::base* &_cam,
	sge::renderer::scalar _opacity,
	sge::image3d::view::const_object const &_tex,
	sge::image3d::view::const_object const &_noise,
	sge::renderer::texture::planar_shared_ptr _phase_tex)
:
renderer_(
	_renderer),
vd_(
	renderer_.create_vertex_declaration(
		sge::renderer::vf::dynamic::make_format<vf::format>())),
vb_(
	renderer_.create_vertex_buffer(
		*vd_,
		sge::renderer::vf::dynamic::part_index(0u),
		// Two triangles per face
		sge::renderer::vertex_count(
			2u * 3u * 6u),
		sge::renderer::resource_flags::none)),
cam_(
	_cam),
opacity_(
	_opacity),
tex_(
	_tex),
noise_(
	_noise),
phase_tex_(
	_phase_tex),
shader_(
	sge::shader::object_parameters(
		renderer_,
		*vd_,
		sge::shader::vf_to_string<sgevollib::cube::vf::format>(),
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
				"opacity",
				sge::shader::variable_type::uniform,
				_opacity))
			(sge::shader::variable(
				"offset",
				sge::shader::variable_type::uniform,
				static_cast<sge::renderer::scalar>(
					0.f)))
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
				"phasetex", phase_tex_))
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
	// Um lesend oder schreibend auf einen Vertexbuffer zugreifen zu können, muss
	// man ihn locken. Intern wird da vermutlich der Speicherblock, der sich auf
	// der Graka befindet, in den Hauptspeicher übertragen, damit man den
	// auslesen kann.
	sge::renderer::scoped_vertex_lock const vblock(
		*vb_,
		sge::renderer::lock_mode::writeonly);

	// Ein Vertexbuffer ist eigentlich nur ein roher Block Speicher. Hier
	// erstellen wir einen "View" auf diesen Block, d.h. versehen ihn mit einer
	// Struktur. Ab da können wir auf den Speicher zugreifen, als sei er mit dem
	// Vertexformat gefüllt.
	vf::vertex_view const vertices(
		vblock.value());

	vf::vertex_view::iterator vb_it(
		vertices.begin());

	// copypaste (macht aus vf::vector einen fcppt::math::vector)
	typedef
	vf::position::packed_type
	position_vector;

	// Here be dragons...
	int a = -1;
	int b = 1;

	for (unsigned side = 0u; side < 6u; ++side)
		for (int tri = 0; tri < 2; ++tri)
			for (int i = 0; i < 3; ++i)
			{
				int vert = (tri == 0) ? i : 2 - i;
				unsigned x = side % 3u;
				int y = (vert == 0) ? a : b;
				int z = (vert == 2) ? b : a;
				if (
					((side % 2 == 0) && tri == 1) ||
					((side % 2 == 1) && tri == 0))
				{
					std::swap(y,z);
				}
				position_vector res(0,0,0);
				res[x] = static_cast<position_vector::value_type>(
					(side > x) ? b : a);
				res[x == 0u ? 1u : 0u] = static_cast<position_vector::value_type>(
					y);
				res[x == 2u ? 1u : 2u] = static_cast<position_vector::value_type>(
					z);

				(*vb_it++).set<vf::position>(
					res);
			}

	sge::renderer::texture::volume_shared_ptr shadertex(
		sge::renderer::texture::create_volume_from_view(
			renderer_,
			tex_,
			sge::renderer::texture::mipmap::all_levels(
				sge::renderer::texture::mipmap::auto_generate::yes),
			sge::renderer::resource_flags::none));

	shader_.update_texture(
		"tex",
		shadertex);

	shadertex = sge::renderer::texture::volume_shared_ptr(
		sge::renderer::texture::create_volume_from_view(
			renderer_,
			noise_,
			sge::renderer::texture::mipmap::all_levels(
				sge::renderer::texture::mipmap::auto_generate::yes),
			sge::renderer::resource_flags::none));

	shader_.update_texture(
		"noise",
		shadertex);
}

sgevollib::cube::object::~object()
{
}

void
sgevollib::cube::object::render(float offset)
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

	if(
			std::abs(cam_->coordinate_system().position().get().x()) >= 1.0f ||
			std::abs(cam_->coordinate_system().position().get().y()) >= 1.0f ||
			std::abs(cam_->coordinate_system().position().get().z()) >= 1.0f
		)
	{
		renderer_.state(
			sge::renderer::state::list(
				sge::renderer::state::cull_mode::clockwise
			));
	}
	else
	{
		renderer_.state(
			sge::renderer::state::list(
				sge::renderer::state::cull_mode::counter_clockwise
			));
	}
	/*
	renderer_.state(
		sge::renderer::state::list(
			sge::renderer::state::cull_mode::clockwise));
	*/

	shader_.update_uniform(
		"mvp",
		sge::shader::matrix(
		cam_->projection_matrix().get(),
		sge::shader::matrix_flags::projection));

	shader_.update_uniform(
		"camera",
		cam_->coordinate_system().position().get());

	shader_.update_uniform(
		"opacity",
		opacity_);

	shader_.update_uniform(
		"offset",
		offset);

	shader_.update_uniform(
		"mv",
		sge::shader::matrix(
		sge::camera::matrix_conversion::world(cam_->coordinate_system()),
		sge::shader::matrix_flags::none));
}
