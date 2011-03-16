#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scoped_vertex_lock.hpp>
#include <sge/shader/object.hpp>
#include <sge/shader/scoped.hpp>
#include <utility>
#include "vf.hpp"
#include "create_cube.hpp"

namespace sgevol
{
// Hier ein kurzes Wort zu Vertexbuffern: Das sind letztendlich nur
// Speicherbereiche auf dem Graka-RAM.
std::pair<sge::renderer::vertex_buffer_ptr,sge::renderer::vertex_declaration_ptr> const
create_cube(
	sge::renderer::device_ptr const renderer,
	sge::shader::object &sh)
{
	// Um den Vertexbuffer zu befüllen, müssen wir erstmal den Shader aktivieren,
	// weil wir unspecified-Attribute im Vertexformat haben. 
	sge::shader::scoped scoped_shader(
		sh);

	sge::renderer::vertex_declaration_ptr const decl =
		renderer->create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<vf::format>());

	sge::renderer::vertex_buffer_ptr const vb = 
		renderer->create_vertex_buffer(
			decl,
			// copypaste
			sge::renderer::vf::dynamic::part_index(
				0u),
			// Two triangles per slice
			static_cast<sge::renderer::size_type>(
				6 * 6 ),
			// Hier kann man angeben, ob der Vertexbuffer "dynamic" sein soll, oder
			// nicht. Dynamische Vertexbuffer sind solche, die man oft (jedes Frame
			// z.B.) neu setzen will. Hier brauchen wir das aber offenbar nicht.
			sge::renderer::resource_flags::none);

	// Um lesend oder schreibend auf einen Vertexbuffer zugreifen zu können, muss
	// man ihn locken. Intern wird da vermutlich der Speicherblock, der sich auf
	// der Graka befindet, in den Hauptspeicher übertragen, damit man den
	// auslesen kann.
	sge::renderer::scoped_vertex_lock const vblock(
		vb,
		sge::renderer::lock_mode::writeonly);

	// Ein Vertexbuffer ist eigentlich nur ein roher Block Speicher. Hier
	// erstellen wir einen "View" auf diesen Block, d.h. versehen ihn mit einer
	// Struktur. Ab da können wir auf den Speicher zugreifen, als sei er mit dem
	// Vertexformat gefüllt.
	vf::vertex_view const vertices(
		vblock.value());

	// Iterator auf den Anfang des Vertexbuffers erstellen
	vf::vertex_view::iterator vb_it(
		vertices.begin());

	// copypaste (macht aus vf::vector einen fcppt::math::vector)
	typedef 
	vf::position::packed_type 
	position_vector;

	// VB befüllen!
	//	sge::renderer::scalar const z = 
	//		1.0f - static_cast<sge::renderer::scalar>(i)/static_cast<sge::renderer::scalar>(slices);
	// Hier setzen wir für alle Vertizes alle Attribute (in unserem Fall nur das Attribut vf::position)
	// unten 1
	(*vb_it++).set<vf::position>(
		position_vector(
			-1,-1,-1));
	(*vb_it++).set<vf::position>(
		position_vector(
			-1,-1,1));
	(*vb_it++).set<vf::position>(
		position_vector(
			1,-1,1));
	// unten 2
	(*vb_it++).set<vf::position>(
		position_vector(
			1,-1,1));
	(*vb_it++).set<vf::position>(
		position_vector(
			1,-1,-1));
	(*vb_it++).set<vf::position>(
		position_vector(
			-1,-1,-1));
	// oben 1
	(*vb_it++).set<vf::position>(
		position_vector(
			-1,1,-1));
	(*vb_it++).set<vf::position>(
		position_vector(
			1,1,-1));
	(*vb_it++).set<vf::position>(
		position_vector(
			1,1,1));
	// oben 2
	(*vb_it++).set<vf::position>(
		position_vector(
			1,1,1));
	(*vb_it++).set<vf::position>(
		position_vector(
			-1,1,1));
	(*vb_it++).set<vf::position>(
		position_vector(
			-1,1,-1));
	// links 1
	(*vb_it++).set<vf::position>(
		position_vector(
			-1,-1,-1));
	(*vb_it++).set<vf::position>(
		position_vector(
			-1,1,-1));
	(*vb_it++).set<vf::position>(
		position_vector(
			-1,1,1));
	// links 2
	(*vb_it++).set<vf::position>(
		position_vector(
			-1,1,1));
	(*vb_it++).set<vf::position>(
		position_vector(
			-1,-1,1));
	(*vb_it++).set<vf::position>(
		position_vector(
			-1,-1,-1));
	// rechts 1
	(*vb_it++).set<vf::position>(
		position_vector(
			1,-1,-1));
	(*vb_it++).set<vf::position>(
		position_vector(
			1,-1,1));
	(*vb_it++).set<vf::position>(
		position_vector(
			1,1,1));
	// rechts 2
	(*vb_it++).set<vf::position>(
		position_vector(
			1,1,1));
	(*vb_it++).set<vf::position>(
		position_vector(
			1,1,-1));
	(*vb_it++).set<vf::position>(
		position_vector(
			1,-1,-1));
	// vorne 1
	(*vb_it++).set<vf::position>(
		position_vector(
			-1,-1,1));
	(*vb_it++).set<vf::position>(
		position_vector(
			-1,1,1));
	(*vb_it++).set<vf::position>(
		position_vector(
			1,1,1));
	// vorne 2
	(*vb_it++).set<vf::position>(
		position_vector(
			1,1,1));
	(*vb_it++).set<vf::position>(
		position_vector(
			1,-1,1));
	(*vb_it++).set<vf::position>(
		position_vector(
			-1,-1,1));
	// hinten 1
	(*vb_it++).set<vf::position>(
		position_vector(
			-1,-1,-1));
	(*vb_it++).set<vf::position>(
		position_vector(
			1,-1,-1));
	(*vb_it++).set<vf::position>(
		position_vector(
			1,1,-1));
	// hinten 2
	(*vb_it++).set<vf::position>(
		position_vector(
			1,1,-1));
	(*vb_it++).set<vf::position>(
		position_vector(
			-1,1,-1));
	(*vb_it++).set<vf::position>(
		position_vector(
			-1,-1,-1));

	return std::make_pair(vb,decl);
}

}
