#ifndef SGEVOLLIB_VF_HPP_INCLUDED
#define SGEVOLLIB_VF_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <sge/renderer/vf/vf.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>


namespace sgevollib
{
namespace vf
{
// Das Vertexformat regelt, welche Daten an einem Vertex kleben. Bei uns ist
// das nur ein 3D-Vektor, der die Position des Vertex angibt. In OpenGL-2 und
// kleiner gab es noch "besondere" Vertexdaten wie "Position", "Farbe",
// "Texturkoordinate", die die fest eingebauten Shader zur Verarbeitung benutzt
// haben.
//
// In OpenGL-3 gibt es nur noch "rohe" Attribute für die Vertizes. Das
// heißt bei sge "unspecified". Jedes Attribut hat einen Namen, den man mit dem
// SGE_RENDERER_VF_MAKE_UNSPECIFIED_TAG festlegt (in unserem Fall heißt
// das Attribut also "position"). Dieser Name taucht in den Shadern wieder auf,
// wo im Header steht:
//
// in <Typ> <Name>;
namespace tags
{
SGE_RENDERER_VF_MAKE_UNSPECIFIED_TAG(position);
}

typedef
sge::renderer::vf::unspecified
<
	sge::renderer::vf::vector
	<
		// renderer::scalar ist ein float
		sge::renderer::scalar,
		// 3 Komponenten
		3
	>,
	tags::position
>
position;

// Das eigentliche Format
typedef
sge::renderer::vf::part
<
	boost::mpl::vector1
	<
		position
	>
>
part;

typedef
sge::renderer::vf::format
<
	boost::mpl::vector1
	<
		part
	>
>
format;

// Das brauchen wir später um den Vertexbuffer zu befüllen.
typedef
sge::renderer::vf::view
<
	part
>
vertex_view;

}
}

#endif
