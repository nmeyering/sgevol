#ifndef SGEVOL_CREATE_CUBE_HPP_INCLUDED
#define SGEVOL_CREATE_CUBE_HPP_INCLUDED

#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/renderer/vertex_declaration_ptr.hpp>
#include <sge/renderer/device_ptr.hpp>
#include <sge/shader/object.hpp>

namespace sgevol
{
std::pair<sge::renderer::vertex_buffer_ptr,sge::renderer::vertex_declaration_ptr> const
create_cube(
	sge::renderer::device_ptr const,
	sge::shader::object&);
}

#endif
