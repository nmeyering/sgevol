#include <sge/shader/shader.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/vector3.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/filesystem/path.hpp>
#include "media_path.hpp"
#include "vf.hpp"
#include "create_shader.hpp"

namespace sgevol
{
fcppt::shared_ptr<sge::shader::object>
create_shader(
	sge::renderer::device &rend,
	fcppt::filesystem::path const &fragmentpath)
{
	return fcppt::shared_ptr<sge::shader::object>(
		new sge::shader::object(
		rend,
		// Vertexshader, liegen in unserem Fall im sge-Mediapath
		sgevol::media_path()
			/ FCPPT_TEXT("shaders")
			/ FCPPT_TEXT("vertex")
			/ FCPPT_TEXT("vertex.glsl"),
		// Fragmentshader
		fragmentpath,
		// Das hier nimmt sich das Vertexformat (vf::format) und macht das zu einer
		// glsl-Deklaration aller Variablen, d.h. in unserem Fall erstellt das
		// automatisch:
		//
		// in vec3 position;
		//
		// Deshalb sind diese $$$HEADER$$$ in den Shadern. Wenn man jetzt ein
		// Vertexattribut hinzufügt, muss man am Shader nichts mehr ändern!
		sge::shader::vf_to_string<sgevol::vf::format>(),
		// Alle uniform-Variablen (und Konstanten), die man im Shader nutzen will.
		// Wir brauchen nur die Modelviewprojection-Matrix von der Kamera im Shader.
		// Diese Liste wird auch für $$$HEADER$$$ rangezogen
		fcppt::assign::make_container<sge::shader::variable_sequence>
			(sge::shader::variable(
				// Name der Variable
				"mvp",
				// Typ (uniform oder const_ für Konstanten)
				sge::shader::variable_type::uniform,
				// Wir nehmen wir eine leere Matrix, wir setzen die jedes Frame neu mit der Kamera
				sge::renderer::matrix4()))
			(sge::shader::variable(
				// Name der Variable
				"mv",
				// Typ (uniform oder const_ für Konstanten)
				sge::shader::variable_type::uniform,
				// Wir nehmen wir eine leere Matrix, wir setzen die jedes Frame neu mit der Kamera
				sge::renderer::matrix4()))
			(sge::shader::variable(
				"offset",
				sge::shader::variable_type::uniform,
				static_cast<sge::renderer::scalar>(
					0.f)))
			(sge::shader::variable(
				"light",
				sge::shader::variable_type::uniform,
				static_cast<sge::renderer::scalar>(
					0.f)))
			(sge::shader::variable(
				"camera",
				sge::shader::variable_type::uniform,
				sge::renderer::vector3())),
		// sampler (sprich Texturen), die wir im Shader brauchen (ebenfalls in $$$HEADER$$$ drin)
		fcppt::assign::make_container<sge::shader::sampler_sequence>
			(sge::shader::sampler(
				"tex", sge::renderer::texture::volume_ptr()
				// Selbsterklärend
				// Man muss bei 3D-Texturen noch angeben, dass die 3 Dimensionen hat.
				// Das kann er aus dem obigen create_volume_texture leider nicht
				// ableiten (ein TODO für Freundlich?)
				))));
}
}
