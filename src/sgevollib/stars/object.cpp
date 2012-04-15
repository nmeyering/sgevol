#include <sgevollib/vf.hpp>
#include <sgevollib/stars/object.hpp>
#include <sge/camera/base.hpp>
#include <sge/camera/coordinate_system/object.hpp>
#include <sge/camera/matrix_conversion/rotation.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/first_vertex.hpp>
#include <sge/renderer/lock_mode.hpp>
#include <sge/renderer/matrix4.hpp>
#include <sge/renderer/nonindexed_primitive_type.hpp>
#include <sge/renderer/resource_flags.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scalar.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/scoped_vertex_lock.hpp>
#include <sge/renderer/size_type.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vertex_count.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/state/cull_mode.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/state/trampoline.hpp>
#include <sge/renderer/vf/iterator.hpp>
#include <sge/renderer/vf/vertex.hpp>
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
#include <fcppt/assign/make_container.hpp>
#include <fcppt/math/twopi.hpp>
#include <fcppt/math/matrix/arithmetic.hpp>
#include <fcppt/math/vector/hypersphere_to_cartesian.hpp>
#include <fcppt/random/variate.hpp>
#include <fcppt/random/distribution/uniform_real.hpp>
#include <fcppt/random/generator/minstd_rand.hpp>
#include <fcppt/random/generator/seed_from_chrono.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/path.hpp>
#include <cmath>
#include <fcppt/config/external_end.hpp>


sgevollib::stars::object::object(
	sge::renderer::size_type _count,
	sge::renderer::scalar _max_size,
	sge::renderer::device &_renderer,
	boost::filesystem::path const &_vertex_shader_file,
	boost::filesystem::path const &_fragment_shader_file,
	sge::camera::base &_camera)
:
count_(
	_count),
max_size_(
	_max_size),
renderer_(
	_renderer),
vd_(
	renderer_.create_vertex_declaration(
		sge::renderer::vf::dynamic::make_format<sgevollib::stars::vf::format>())),
vb_(
	renderer_.create_vertex_buffer(
		*vd_,
		sge::renderer::vf::dynamic::part_index(0u),
		sge::renderer::vertex_count(count_),
		sge::renderer::resource_flags::none)),
camera_(
	_camera),
shader_(
		sge::shader::object_parameters(
		renderer_,
		*vd_,
		sge::shader::vf_to_string<vf::format>(),
		fcppt::assign::make_container<sge::shader::variable_sequence>
			(sge::shader::variable(
				"mvp",
				sge::shader::variable_type::uniform,
				sge::shader::matrix(
					sge::renderer::matrix4::identity(),
					sge::shader::matrix_flags::projection))),
		sge::shader::sampler_sequence())
				.vertex_shader(
					_vertex_shader_file)
				.fragment_shader(
					_fragment_shader_file))
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
		3
	>::type
	vec3;

	typedef fcppt::random::generator::minstd_rand default_generator;

	default_generator def_gen(
		fcppt::random::generator::seed_from_chrono<
			default_generator::seed
		>());

	typedef fcppt::random::distribution::uniform_real<
		float
	> scalar_distribution;

	fcppt::random::variate<default_generator, scalar_distribution>
		rng(
			def_gen,
			scalar_distribution(
				scalar_distribution::min(
					-1.f),
				scalar_distribution::sup(
					1.f))),
		angle_rng(
			def_gen,
			scalar_distribution(
				scalar_distribution::min(
					0.f),
				scalar_distribution::sup(
					fcppt::math::twopi<float>()))),
		radius_rng(
			def_gen,
			scalar_distribution(
				scalar_distribution::min(
					1.f),
				scalar_distribution::sup(
					static_cast<float>(max_size_))));

	for (
		vf::vertex_view::iterator vb_it = vertices.begin();
		vb_it != vertices.end();
		++vb_it)
	{
		float z = rng();
		float r = std::sqrt(1 - z*z);
		float angle = angle_rng();
		vb_it->set<vf::position>(
				vec3(
					r * std::cos(angle),
					r * std::sin(angle),
					z)
			);
		vb_it->set<vf::color>(
			color_vector(
				1.f,
				1.f,
				1.f));
		vb_it->set<vf::radius>(
			radius_vector(radius_rng()));
	}
}

sgevollib::stars::object::~object()
{
}

void
sgevollib::stars::object::render()
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
		sge::renderer::first_vertex(0u),
		sge::renderer::vertex_count(vb_->size()),
		sge::renderer::nonindexed_primitive_type::point);

	renderer_.state(
		sge::renderer::state::list(
			sge::renderer::state::cull_mode::clockwise));

	// mvp updaten
	shader_.update_uniform(
		"mvp",
		sge::shader::matrix(
			camera_.projection_matrix().get() *
				sge::camera::matrix_conversion::rotation(camera_.coordinate_system()),
			sge::shader::matrix_flags::projection));
}
