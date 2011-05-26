#ifndef SGEVOL_CREATE_SHADER_HPP_INCLUDED
#define SGEVOL_CREATE_SHADER_HPP_INCLUDED

#include <fcppt/filesystem/path.hpp>
#include <sge/shader/object.hpp>

namespace sgevol
{
fcppt::shared_ptr<sge::shader::object>
create_shader(
	sge::renderer::device &,
	fcppt::filesystem::path const &);
}

#endif
