#ifndef SGEVOL_MODEL_OBJECT_HPP_INCLUDED
#define SGEVOL_MODEL_OBJECT_HPP_INCLUDED

#include <sge/camera/object.hpp>
#include <sge/image3d/view/const_object.hpp>
#include <sge/model/obj/instance_ptr.hpp>
#include <sge/model/obj/loader_ptr.hpp>
#include <sge/model/obj/vb_converter/convert.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/texture/planar_ptr.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/renderer/vertex_declaration_ptr.hpp>
#include <sge/shader/object.hpp>
#include <sgevol/model/vf.hpp>

namespace sgevol
{
namespace model
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
		fcppt::filesystem::path const &,
		fcppt::filesystem::path const &,
		sge::renderer::texture::planar_ptr,
		sge::camera::object &);

	void render();

	~object();
private:
	sge::renderer::device &renderer_;
	sge::model::obj::loader_ptr const model_loader_;
	sge::model::obj::instance_ptr model_;
	sge::renderer::vertex_declaration_ptr const vd_;
	sge::renderer::vertex_buffer_ptr vb_;
	sge::camera::object &cam_;
	sge::renderer::texture::planar_ptr tex_;
	sge::shader::object shader_;
};

}
}
#endif
