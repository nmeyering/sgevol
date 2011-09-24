#ifndef SGEVOL_MODEL_OBJECT_HPP_INCLUDED
#define SGEVOL_MODEL_OBJECT_HPP_INCLUDED

#include <sge/image3d/view/const_object.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/shader/object.hpp>
#include <sge/camera/object.hpp>
#include <sge/model/obj/instance_ptr.hpp>
#include <sge/model/obj/loader_ptr.hpp>
#include <sge/renderer/texture/planar_ptr.hpp>
#include <sge/renderer/vertex_declaration_ptr.hpp>

#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/map/map10.hpp>
#include <boost/mpl/pair.hpp>
#include <fcppt/mpl/index_of.hpp>
#include <fcppt/config/external_end.hpp>

namespace sgevol
{
namespace model
{
class object
{
FCPPT_NONCOPYABLE(
	object);
private:
	typedef
	boost::mpl::map3
	<
		boost::mpl::pair
		<
			sge::model::obj::vb_converter::roles::position,
			fcppt::mpl::index_of
			<
				vf::part::elements,
				vf::position
			>
		>,
		boost::mpl::pair
		<
			sge::model::obj::vb_converter::roles::texcoord,
			fcppt::mpl::index_of
			<
				vf::part::elements,
				vf::texcoord
			>
		>,
		boost::mpl::pair
		<
			sge::model::obj::vb_converter::roles::normal,
			fcppt::mpl::index_of
			<
				vf::part::elements,
				vf::normal
			>
		>
	>
	vertex_format;
public:
	explicit
	object(
		sge::renderer::device &,
		fcppt::filesystem::path const &,
		fcppt::filesystem::path const &,
		sge::renderer::texture::planar_ptr,
		sge::camera::object &);

	void render();

	~object();
private:
	sge::renderer::device &renderer_;
	sge::renderer::vertex_declaration_ptr const vd_;
	sge::renderer::vertex_buffer_ptr vb_;
	sge::renderer::texture::planar_ptr tex_;
	sge::camera::object &cam_;
	sge::shader::object shader_;
	sge::model::obj::loader_ptr const model_loader_;
	sge::model::obj::instance_ptr model_;
};

}
}
#endif
