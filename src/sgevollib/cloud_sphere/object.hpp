#ifndef SGEVOLLIB_CUBE_OBJECT_HPP_INCLUDED
#define SGEVOLLIB_CUBE_OBJECT_HPP_INCLUDED

#include <sge/camera/base.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/image3d/view/const_object.hpp>
#include <sge/model/obj/instance.hpp>
#include <sge/model/obj/vb_converter/convert.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/texture/planar_shared_ptr.hpp>
#include <sge/renderer/vertex_buffer_shared_ptr.hpp>
#include <sge/renderer/vertex_declaration_shared_ptr.hpp>
#include <sge/renderer/glsl/uniform/int_type.hpp>
#include <sge/shader/object.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/path.hpp>
#include <fcppt/config/external_end.hpp>
#include <sge/renderer/scalar.hpp>


namespace sgevollib
{
namespace cloud_sphere
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	explicit
	object(
		sge::renderer::device &,
		sge::model::obj::instance &,
		boost::filesystem::path const &_vertex_shader_file,
		boost::filesystem::path const &_fragment_shader_file,
		sge::camera::base* &,
		sge::renderer::scalar,
		sge::renderer::scalar,
		sge::renderer::glsl::uniform::int_type,
		sge::image3d::view::const_object const &,
		sge::image3d::view::const_object const &,
		sge::renderer::texture::planar_shared_ptr &);

	void render(float);

	void opacity(
		sge::renderer::scalar const _opacity)
	{
		opacity_ =
			_opacity;
	}

	sge::renderer::scalar
	opacity()
	const
	{
		return opacity_;
	}

	void
	skip(
		sge::renderer::glsl::uniform::int_type _skip)
	{
		skip_ =
			_skip;
	}

	sge::renderer::glsl::uniform::int_type
	skip()
	{
		return skip_;
	}

	~object();
private:
	sge::renderer::device &renderer_;
	sge::model::obj::instance &model_;
	sge::renderer::vertex_declaration_shared_ptr vd_;
	sge::renderer::vertex_buffer_shared_ptr vb_;
	sge::camera::base* &cam_;
	sge::renderer::scalar opacity_;
	sge::renderer::glsl::uniform::int_type skip_;
	sge::image3d::view::const_object const &tex_;
	sge::image3d::view::const_object const &noise_;
	sge::renderer::texture::planar_shared_ptr &clouds_;
	sge::shader::object shader_;
};

}
}
#endif
