#include <sgevol/vf.hpp>
#include <sgevol/stars/object.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/filesystem/path.hpp>
#include <fcppt/math/matrix/arithmetic.hpp>
#include <fcppt/math/twopi.hpp>
#include <fcppt/math/vector/hypersphere_to_cartesian.hpp>
#include <fcppt/random/make_inclusive_range.hpp>
#include <fcppt/random/uniform.hpp>
#include <sge/camera/object.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/first_vertex.hpp>
#include <sge/renderer/lock_mode.hpp>
#include <sge/renderer/matrix4.hpp>
#include <sge/renderer/nonindexed_primitive_type.hpp>
#include <sge/renderer/resource_flags.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/scoped_vertex_lock.hpp>
#include <sge/renderer/size_type.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/state/trampoline.hpp>
#include <sge/renderer/state/cull_mode.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vertex_count.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/renderer/vf/dynamic/part_index.hpp>
#include <sge/shader/activate_everything.hpp>
#include <sge/shader/matrix.hpp>
#include <sge/shader/matrix_flags.hpp>
#include <sge/shader/object_parameters.hpp>
#include <sge/shader/sampler_sequence.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/variable.hpp>
#include <sge/shader/variable_sequence.hpp>
#include <sge/shader/variable_type.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <cmath>

sgevol::stars::object::object(
	sge::renderer::size_type _count,
	sge::renderer::scalar _max_size,
	sge::renderer::device &_renderer,
	fcppt::filesystem::path const &_vertex_shader_file,
	fcppt::filesystem::path const &_fragment_shader_file,
	sge::camera::object &_camera)
:
count_(
	_count),
max_size_(
	_max_size),
renderer_(
	_renderer),
vd_(
	renderer_.create_vertex_declaration(
		sge::renderer::vf::dynamic::make_format<sgevol::stars::vf::format>())),
vb_(
	renderer_.create_vertex_buffer(
		*vd_,
		sge::renderer::vf::dynamic::part_index(0u),
		count_,
		sge::renderer::resource_flags::none)),
camera_(
	_camera),
shader_(
		sge::shader::object_parameters(
		renderer_,
		*vd_,
		_vertex_shader_file,
		_fragment_shader_file,
		sge::shader::vf_to_string<vf::format>(),
		fcppt::assign::make_container<sge::shader::variable_sequence>
			(sge::shader::variable(
				"mvp",
				sge::shader::variable_type::uniform,
				sge::shader::matrix(
					sge::renderer::matrix4::identity(),
					sge::shader::matrix_flags::projection))),
		sge::shader::sampler_sequence()
				))
{
	sge::renderer::scoped_vertex_lock const vblock(
		*vb_,
		sge::renderer::lock_mode::writeonly);

	vf::vertex_view const vertices(
		vblock.value());

	typedef
	vf::position::packed_type
	position_vector;

	typedef
	vf::color::packed_type
	color_vector;

	typedef
	vf::radius::packed_type
	radius_vector;

	typedef
	fcppt::math::vector::static_<
		float,
		2
	>::type
	vec2;

	fcppt::random::uniform<float> color_rng(
		fcppt::random::make_inclusive_range(
			0.0f,
			1.f));
		
	fcppt::random::uniform<float> angle_rng(
		fcppt::random::make_inclusive_range(
			0.f,
			1.f));

	fcppt::random::uniform<sge::renderer::scalar> radius_rng(
		fcppt::random::make_inclusive_range(
			static_cast<sge::renderer::scalar>(1.f),
			max_size_));

	for (
		vf::vertex_view::iterator vb_it = vertices.begin();
		vb_it != vertices.end();
		++vb_it)
	{
		vb_it->set<vf::position>(
			fcppt::math::vector::hypersphere_to_cartesian(
				vec2(
					fcppt::math::twopi<float>() * angle_rng(),
					std::acos(2.f * angle_rng() - 1.f)))
			);
		vb_it->set<vf::color>(
			color_vector(
				color_rng(),
				color_rng(),
				color_rng()));
		vb_it->set<vf::radius>(
			radius_vector(radius_rng()));
	}
}

sgevol::stars::object::~object()
{
}

void
sgevol::stars::object::render()
{
	sge::renderer::state::scoped scoped_state(
		renderer_,
		sge::renderer::state::list(
			sge::renderer::state::bool_::enable_point_sprites = true));

	sge::shader::scoped scoped_shader(
		shader_,
		sge::shader::activate_everything());

	sge::renderer::scoped_vertex_declaration const decl_context(
		renderer_,
		*vd_);

	sge::renderer::scoped_vertex_buffer const vb_context(
		renderer_,
		*vb_);

	renderer_.render_nonindexed(
		sge::renderer::first_vertex(0),
		sge::renderer::vertex_count(vb_->size()),
		sge::renderer::nonindexed_primitive_type::point);

	renderer_.state(
		sge::renderer::state::list(
			sge::renderer::state::cull_mode::clockwise));

	// mvp updaten
	shader_.update_uniform(
		"mvp",
		sge::shader::matrix(
		camera_.projection() * camera_.rotation(),
		sge::shader::matrix_flags::projection));
}
