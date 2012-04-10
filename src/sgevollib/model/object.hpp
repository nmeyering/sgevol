#ifndef SGEVOLLIB_MODEL_OBJECT_HPP_INCLUDED
#define SGEVOLLIB_MODEL_OBJECT_HPP_INCLUDED

#include <sgevollib/model/vf.hpp>
#include <sge/camera/base.hpp>
#include <sge/image3d/view/const_object.hpp>
#include <sge/model/obj/instance.hpp>
//#include <sge/model/obj/loader_scoped_ptr.hpp>
#include <sge/model/obj/vb_converter/convert.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/vertex_buffer_shared_ptr.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/renderer/vertex_declaration_shared_ptr.hpp>
#include <sge/renderer/texture/planar_shared_ptr.hpp>
#include <sge/shader/object.hpp>
#include <sge/renderer/scalar.hpp>

namespace sgevollib
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
		sge::model::obj::instance &,
		boost::filesystem::path const &,
		boost::filesystem::path const &,
		sge::renderer::texture::planar_shared_ptr,
		float _radius,
		sge::camera::base* &);

	void render(float);

	~object();
private:
	sge::renderer::device &renderer_;
	//sge::model::obj::loader_scoped_ptr const model_loader_;
	sge::model::obj::instance &model_;
	sge::renderer::vertex_declaration_shared_ptr const vd_;
	sge::renderer::vertex_buffer_shared_ptr vb_;
	sge::renderer::texture::planar_shared_ptr tex_;
	sge::renderer::scalar radius_;
	sge::camera::base* &cam_;
	sge::shader::object shader_;
};

}
}
#endif
