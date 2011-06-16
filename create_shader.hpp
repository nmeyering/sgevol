#ifndef SGEVOL_CREATE_SHADER_HPP_INCLUDED
#define SGEVOL_CREATE_SHADER_HPP_INCLUDED

#include <fcppt/shared_ptr.hpp>
#include <fcppt/filesystem/path.hpp>
#include <sge/renderer/vertex_declaration_ptr.hpp>
#include <sge/shader/object.hpp>

namespace sgevol
{
fcppt::shared_ptr<sge::shader::object>
create_shader(
	sge::renderer::device &,
	sge::renderer::vertex_declaration_ptr const &,
	fcppt::filesystem::path const &);
}

#endif
