#ifndef SGEVOL_CUBE_OBJECT_HPP_INCLUDED
#define SGEVOL_CUBE_OBJECT_HPP_INCLUDED

#include <sge/image3d/view/const_object.hpp>
#include <sge/model/obj/instance_ptr.hpp>
#include <sge/model/obj/loader_ptr.hpp>
#include <sge/model/obj/vb_converter/convert.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/shader/object.hpp>
#include <sge/camera/base.hpp>

namespace sgevol
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
		sge::model::obj::instance_ptr,
		fcppt::filesystem::path const &_vertex_shader_file,
		fcppt::filesystem::path const &_fragment_shader_file,
		sge::camera::base* &,
		sge::renderer::scalar radius,
		sge::renderer::scalar opacity,
		sge::image3d::view::const_object const &_tex,
		sge::image3d::view::const_object const &_noise);

	void render();

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

	~object();
private:
	sge::renderer::device &renderer_;
	sge::model::obj::instance_ptr model_;
	sge::renderer::vertex_declaration_ptr vd_;
	sge::renderer::vertex_buffer_ptr vb_;
	sge::camera::base* &cam_;
	sge::renderer::scalar opacity_;
	sge::image3d::view::const_object const &tex_;
	sge::image3d::view::const_object const &noise_;
	sge::shader::object shader_;
};

}
}
#endif
