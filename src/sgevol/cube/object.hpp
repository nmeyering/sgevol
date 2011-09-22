#ifndef SGEVOL_CUBE_OBJECT_HPP_INCLUDED
#define SGEVOL_CUBE_OBJECT_HPP_INCLUDED

#include <sge/image3d/view/const_object.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/shader/object.hpp>
#include <sge/camera/object.hpp>

namespace sgevol
{
namespace cube
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	explicit
	object(
		sge::renderer::device &,
		fcppt::filesystem::path const &,
		sge::camera::object &,
		sge::image3d::view::const_object const &);
	
	void render();

	~object();
private:
	sge::renderer::device &renderer_;
	sge::renderer::vertex_declaration_ptr vd_;
	sge::renderer::vertex_buffer_ptr vb_;
	sge::camera::object &cam_;
	fcppt::shared_ptr<sge::shader::object> shader_;
	sge::image3d::view::const_object const &tex_;
	fcppt::shared_ptr<sge::shader::object> create_shader(
		fcppt::filesystem::path const &);
};

}
}
#endif
