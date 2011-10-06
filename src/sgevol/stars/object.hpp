#ifndef SGEVOL_STARS_OBJECT_HPP_INCLUDED
#define SGEVOL_STARS_OBJECT_HPP_INCLUDED

#include <sgevol/stars/vf.hpp>
#include <fcppt/filesystem/path.hpp>
#include <sge/camera/base.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/size_type.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/renderer/vertex_declaration_ptr.hpp>
#include <sge/shader/object.hpp>

namespace sgevol
{
namespace stars
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	explicit
	object(
		sge::renderer::size_type,
		sge::renderer::scalar,
		sge::renderer::device &,
		fcppt::filesystem::path const &_fragment_shader_file,
		fcppt::filesystem::path const &_vertex_shader_file,
		sge::camera::base &);

	void render();

	~object();
private:
	sge::renderer::size_type count_;
	sge::renderer::scalar max_size_;
	sge::renderer::device &renderer_;
	sge::renderer::vertex_declaration_ptr vd_;
	sge::renderer::vertex_buffer_ptr vb_;
	sge::camera::base &camera_;
	sge::shader::object shader_;
};

}
}
#endif
