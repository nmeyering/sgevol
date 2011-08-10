#include <fcppt/math/vector/vector.hpp> // wegen position::packed_type - wtf?
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scoped_vertex_lock.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <utility>
#include "create_cube.hpp"
#include "vf.hpp"

namespace sgevol
{
// Hier ein kurzes Wort zu Vertexbuffern: Das sind letztendlich nur
// Speicherbereiche auf dem Graka-RAM.
std::pair<sge::renderer::vertex_buffer_ptr,sge::renderer::vertex_declaration_ptr> const
create_cube(
	sge::renderer::device &renderer)
{
	sge::renderer::vertex_declaration_ptr const decl(
		renderer.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<vf::format>()));

	sge::renderer::vertex_buffer_ptr const vb(
		renderer.create_vertex_buffer(
			*decl,
			// copypaste
			sge::renderer::vf::dynamic::part_index(0u),
			// Two triangles per face
			static_cast<sge::renderer::size_type>(
				2 * 3 * 6 ),
			// Hier kann man angeben, ob der Vertexbuffer "dynamic" sein soll, oder
			// nicht. Dynamische Vertexbuffer sind solche, die man oft (jedes Frame
			// z.B.) neu setzen will. Hier brauchen wir das aber offenbar nicht.
			sge::renderer::resource_flags::none));

	// Um lesend oder schreibend auf einen Vertexbuffer zugreifen zu können, muss
	// man ihn locken. Intern wird da vermutlich der Speicherblock, der sich auf
	// der Graka befindet, in den Hauptspeicher übertragen, damit man den
	// auslesen kann.
	sge::renderer::scoped_vertex_lock const vblock(
		*vb,
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

	int a = -1;
	int b = 1;

	for (int side = 0; side < 6; ++side)
		for (int tri = 0; tri < 2; ++tri)
			for (int i = 0; i < 3; ++i)
			{
				int vert = (tri == 0) ? i : 2 - i;
				int x = side % 3;
				int y = (vert == 0) ? a : b;
				int z = (vert == 2) ? b : a;
				if (
					(side % 2 == 0) && tri == 1 ||
					(side % 2 == 1) && tri == 0)
				{
					int t = y;
					y = z;
					z = t;
				}
				position_vector res(0,0,0);
				res[x] = (side > x) ? b : a;
				res[x == 0 ? 1 : 0] = y;
				res[x == 2 ? 1 : 2] = z;

				(*vb_it++).set<vf::position>(
					res);
			}

	return std::make_pair(vb,decl);
}

}
