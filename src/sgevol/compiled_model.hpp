#ifndef SGEVOL_COMPILED_MODEL_HPP_INCLUDED
#define SGEVOL_COMPILED_MODEL_HPP_INCLUDED

#include <sge/model/obj/instance.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/texture/planar_ptr.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/renderer/vertex_declaration.hpp>

namespace sgevol
{

class compiled_model
{
FCPPT_NONCOPYABLE(
	compiled_model);
public:
	explicit
	compiled_model(
		sge::model::obj::instance const &,
		sge::renderer::device &,
		sge::renderer::vertex_declaration const &,
		sge::renderer::texture::planar_ptr);

	void
	render();

	sge::renderer::vertex_buffer const &
	vb() const;

	~compiled_model();
private:
	sge::renderer::vertex_declaration const &vd_;
	sge::renderer::vertex_buffer_ptr vb_;
	sge::renderer::texture::planar_ptr texture_;
	sge::renderer::device &renderer_;
};

}
#endif
