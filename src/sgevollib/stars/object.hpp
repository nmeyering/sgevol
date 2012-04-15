#ifndef SGEVOLLIB_STARS_OBJECT_HPP_INCLUDED
#define SGEVOLLIB_STARS_OBJECT_HPP_INCLUDED

#include <sgevollib/stars/vf.hpp>
#include <sge/camera/base.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/size_type.hpp>
#include <sge/renderer/vertex_buffer_shared_ptr.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/renderer/vertex_declaration_shared_ptr.hpp>
#include <sge/shader/object.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/path.hpp>
#include <fcppt/config/external_end.hpp>


namespace sgevollib
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
		boost::filesystem::path const &_fragment_shader_file,
		boost::filesystem::path const &_vertex_shader_file,
		sge::camera::base &);

	void render();

	~object();
private:
	sge::renderer::size_type count_;
	sge::renderer::scalar max_size_;
	sge::renderer::device &renderer_;
	sge::renderer::vertex_declaration_shared_ptr vd_;
	sge::renderer::vertex_buffer_shared_ptr vb_;
	sge::camera::base &camera_;
	sge::shader::object shader_;
};

}
}
#endif
