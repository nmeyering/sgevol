/*
spacegameengine is a portable easy to use game engine written in C++.
Copyright (C) 2006-2010 Carl Philipp Reh (sefi@s-e-f-i.de)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#include <sge/exception.hpp>
#include <sge/systems/instance.hpp>
#include <sge/systems/list.hpp>
#include <sge/systems/viewport/manage_resize.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/renderer/vf/format.hpp>
#include <sge/renderer/vf/pos.hpp>
#include <sge/renderer/vf/color.hpp>
#include <sge/renderer/vf/view.hpp>
#include <sge/renderer/vf/iterator.hpp>
#include <sge/renderer/vf/vertex.hpp>
#include <sge/renderer/state/trampoline.hpp>
#include <sge/renderer/state/cull_mode.hpp>
#include <sge/renderer/state/depth_func.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/float.hpp>
#include <sge/renderer/state/var.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/volume_texture.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/scoped_block.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_lock.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/refresh_rate_dont_care.hpp>
#include <sge/renderer/no_multi_sampling.hpp>
#include <sge/image/color/rgba8_format.hpp>
#include <sge/image/colors.hpp>
#include <sge/image/color/any/convert.hpp>
#include <sge/input/keyboard/action.hpp>
#include <sge/input/keyboard/device.hpp>
#include "media_path.hpp"
#include <sge/window/instance.hpp>
#include <fcppt/signal/scoped_connection.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/math/vector/basic_impl.hpp>
#include <fcppt/text.hpp>
#include <fcppt/container/grid/object.hpp>
#include <fcppt/math/vector/length.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <boost/spirit/home/phoenix/core/reference.hpp>
#include <boost/spirit/home/phoenix/operator/self.hpp>
#include <ostream>
#include <exception>
#include <cstdlib>

#include <sge/renderer/vf/unspecified.hpp>
#include <sge/renderer/vf/vector.hpp>
#include <sge/renderer/scalar.hpp>
#include <sge/renderer/vf/make_unspecified_tag.hpp>
#include <sge/camera/object.hpp>
#include <sge/camera/parameters.hpp>
#include <sge/shader/object.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/variable_type.hpp>
#include <sge/renderer/matrix4.hpp>
#include <sge/shader/variable.hpp>
#include <sge/renderer/state/source_blend_func.hpp>
#include <sge/renderer/state/dest_blend_func.hpp>
#include <sge/shader/sampler.hpp>
#include <sge/image3d/view/optional_pitch.hpp>
#include <sge/image/const_raw_pointer.hpp>
#include <sge/image/raw_pointer.hpp>
#include <sge/image3d/view/make_const.hpp>
#include <sge/image3d/view/format.hpp>
#include <sge/image3d/dim.hpp>
#include <sge/image3d/view/const_object.hpp>
#include <sge/image3d/view/make.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <sge/camera/projection/perspective.hpp>
#include <sge/renderer/aspect.hpp>
#include <sge/renderer/filter/linear.hpp>
#include <sge/renderer/filter/point.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/time/timer.hpp>
#include <sge/time/second.hpp>
#include <fcppt/math/deg_to_rad.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/container/raw_vector.hpp>
#include <fcppt/math/dim/input.hpp>
#include <fcppt/math/dim/output.hpp>
#include <fcppt/io/cifstream.hpp>
#include <fcppt/exception.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <streambuf>
#include <cstddef>

// Hier mal stat dem anonymen Namensraum ein beliebig benannter
namespace testcase
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
SGE_RENDERER_VF_MAKE_UNSPECIFIED_TAG(position)
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
sge::renderer::vf::format
<
	boost::mpl::vector1
	<
		position
	>
> 
format;

// Das brauchen wir später um den Vertexbuffer zu befüllen.
typedef 
sge::renderer::vf::view
<
	format
>
vertex_view;
}

// Hier ein kurzes Wort zu Vertexbuffern: Das sind letztendlich nur
// Speicherbereiche auf dem Graka-RAM.
sge::renderer::vertex_buffer_ptr const
create_cube(
	sge::renderer::device_ptr const renderer,
	sge::shader::object &sh)
{
	// Um den Vertexbuffer zu befüllen, müssen wir erstmal den Shader aktivieren,
	// weil wir unspecified-Attribute im Vertexformat haben. 
	sge::shader::scoped scoped_shader(
		sh);

	sge::renderer::vertex_buffer_ptr const vb = 
		renderer->create_vertex_buffer(
			// copypaste
			sge::renderer::vf::dynamic::make_format<vf::format>(),
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
//  sge::renderer::scalar const z = 
//    1.0f - static_cast<sge::renderer::scalar>(i)/static_cast<sge::renderer::scalar>(slices);
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
      -1,-1,-1));
  (*vb_it++).set<vf::position>(
    position_vector(
      1,-1,-1));
  (*vb_it++).set<vf::position>(
    position_vector(
      1,-1,1));
  // oben 1
  (*vb_it++).set<vf::position>(
    position_vector(
      -1,1,-1));
  (*vb_it++).set<vf::position>(
    position_vector(
      -1,1,1));
  (*vb_it++).set<vf::position>(
    position_vector(
      1,1,1));
  // oben 2
  (*vb_it++).set<vf::position>(
    position_vector(
      -1,1,-1));
  (*vb_it++).set<vf::position>(
    position_vector(
      1,1,-1));
  (*vb_it++).set<vf::position>(
    position_vector(
      1,1,1));
  // links 1
  (*vb_it++).set<vf::position>(
    position_vector(
      -1,-1,-1));
  (*vb_it++).set<vf::position>(
    position_vector(
      -1,-1,1));
  (*vb_it++).set<vf::position>(
    position_vector(
      -1,1,1));
  // links 2
  (*vb_it++).set<vf::position>(
    position_vector(
      -1,-1,-1));
  (*vb_it++).set<vf::position>(
    position_vector(
      -1,1,-1));
  (*vb_it++).set<vf::position>(
    position_vector(
      -1,1,1));
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
      1,-1,-1));
  (*vb_it++).set<vf::position>(
    position_vector(
      1,1,-1));
  (*vb_it++).set<vf::position>(
    position_vector(
      1,1,1));
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
      -1,-1,1));
  (*vb_it++).set<vf::position>(
    position_vector(
      1,-1,1));
  (*vb_it++).set<vf::position>(
    position_vector(
      1,1,1));
  // hinten 1
  (*vb_it++).set<vf::position>(
    position_vector(
      -1,-1,-1));
  (*vb_it++).set<vf::position>(
    position_vector(
      -1,1,-1));
  (*vb_it++).set<vf::position>(
    position_vector(
      1,1,-1));
  // hinten 2
  (*vb_it++).set<vf::position>(
    position_vector(
      -1,-1,-1));
  (*vb_it++).set<vf::position>(
    position_vector(
      1,-1,-1));
  (*vb_it++).set<vf::position>(
    position_vector(
      1,1,-1));

	return vb;
}

class texture3d
{
public:
	explicit
	texture3d(
		std::size_t const dimension);

	sge::image3d::view::const_object const
	view() const;
private:
  typedef
  fcppt::container::grid::object<
    float,
    3
  >
  grid_3d;

	grid_3d grid_;
	std::size_t dimension_;
};

texture3d::texture3d(
	std::size_t const _dimension
	)
:
  grid_(
    grid_3d::dim(
      _dimension,
      _dimension,
      _dimension
      )),
  dimension_(
    _dimension
    )
{

  // Grid füllen mit Quatsch
  for (int x = 0; x < dimension_; ++x)
  {
    for (int y = 0; y < dimension_; ++y)
    {
      for (int z = 0; z < dimension_; ++z)
      {
        float r =
          fcppt::math::vector::length(
            sge::renderer::vector3(
            x - 128,
            y - 128,
            z - 128
            )
          );
        unsigned char value = 0x10;
        if ( r - 64 < 0 )
          value = 0xFF;
        grid_[
          grid_3d::dim(
            x,
            y,
            z)
        ] = value;
      }
    }
  }
  std::cout << counter << "\n";
  /*
	// Liste der Dateien "datei1, datei2, datei3, ..." durchgehen, alle öffnen, Content zum Array hinzufügen
	for (std::size_t current_slice = 1; current_slice <= _slices; ++current_slice)
	{
		// Dateinamen zusammenklauben
		fcppt::filesystem::path const p = 
			directory / (prefix + FCPPT_TEXT(".") + boost::lexical_cast<fcppt::string>(current_slice));

		// cifstream ist dazu da, Bytes auszulesen, nicht Zeichen.
		fcppt::io::cifstream file(
			p);

		if (!file.is_open())
			throw 
				fcppt::exception(
					FCPPT_TEXT("Couldn't open file ")+
					p.string());

		// Das ist nur eine fancy Art und Weise, den kompletten Inhalt einer Datei
		// zu 'nem String zu machen (hab ich auch nur c&pt)
		std::string const file_content(
			(std::istreambuf_iterator<char>(
				file)),
			std::istreambuf_iterator<char>());

		// Wir setzen mal lieber eine gerade Anzahl Bytes voraus, sonst stimmt was
		// mit der Datei nicht.
		FCPPT_ASSERT(
			file_content.size() % 2u == 0);

		// short aus zwei Bytes zusammensetzen
		for (std::string::size_type current_texel = 0; current_texel < file_content.size(); current_texel += 2)
			*current_byte++ = 
				static_cast<byte_vector::value_type>(
					static_cast<double>(file_content[current_texel+1] << 8 | file_content[current_texel])/65536.0 * 255.0);
	}
  */
}

sge::image3d::view::const_object const
texture3d::view() const
{
	// Das hier macht aus einer Sammlung von Bytes eine Struktur, nämlich ein
	// mizuiro::view. Das braucht der Renderer, um eine Textur draus zu
	// generieren. Sollte alles selbsterklärend sein (ausnahmsweise)
	return 
		sge::image3d::view::make_const(
			reinterpret_cast<sge::image::const_raw_pointer>(
				grid_.data()),
			sge::image3d::dim(
        dimension_,
        dimension_,
        dimension_),
			sge::image::color::format::gray8,
			sge::image3d::view::optional_pitch());
}
}

int 
main(
	int argc,
	char*argv[])
try
{
	boost::program_options::options_description desc(
		"To use this sge example, you need a 3D texture somewhere on your file system.\n"
		"The texture must consist of one file per slice and all slice files\n"
		"have to be in the same directory.\n"
		"You can specify said directory with \"directory\". The file names have the\n"
		"format \"prefix.number\" where prefix can be specified via the \"prefix\"\n"
		"option. The numbers start with 1.\n"
		"The slice files themselves consist of 16 bit integers in little endian.\n"
		"No header. Sample images can be found at\n\n"
		"http://www-graphics.stanford.edu/data/voldata/\n\nOther options include");
	desc.add_options()
    ("help", "Produce help message")
    ("directory", boost::program_options::value<fcppt::string>(), "Set the directory where to take the slices from (see above)")
    ("prefix", boost::program_options::value<fcppt::string>(), "Slice prefix (see above)")
    ("slice-count", boost::program_options::value<std::size_t>(), "How many slices are there")
    ("slice-size", boost::program_options::value<std::size_t>(), "How big is one slice")
    ("screen-size", boost::program_options::value<sge::renderer::screen_size>()->default_value(sge::renderer::screen_size(640,480)), "Screen resolution, format: (x,y)");

	boost::program_options::variables_map vm;
	boost::program_options::store(
		boost::program_options::parse_command_line(
			argc, 
			argv, 
			desc), 
		vm);
	boost::program_options::notify(
		vm);    

	if (vm.count("help")) 
	{
		std::cout << desc << "\n";
		return EXIT_SUCCESS;
	}

	// systems::instance ist eine Hilfsklasse, die es einem abnimmt, alle
	// Plugins, die man so braucht, manuell zu laden und zusammenzustecken.
	// Später arbeitet man nur noch mit sys.renderer(), sys.image_loader() usw.
	//
	// Das sind alles shared_ptr, d.h. man muss auf die Methoden mit
	// sys.renderer()->bla zugreifen statt sys.renderer().bla
	sge::systems::instance const sys(
		sge::systems::list()
		(
			sge::systems::window(
				sge::renderer::window_parameters(
					// Fenstertitel offenbar
					FCPPT_TEXT("sge test for 3d textures")
				)
			)
		)
		(
			sge::systems::renderer(
				sge::renderer::parameters(
					sge::renderer::display_mode(
						// screen-size aus der Kommandozeile
						vm["screen-size"].as<sge::renderer::screen_size>(),
						sge::renderer::bit_depth::depth32,
						sge::renderer::refresh_rate_dont_care
					),
					sge::renderer::depth_buffer::d24,
					sge::renderer::stencil_buffer::off,
					sge::renderer::window_mode::windowed,
					sge::renderer::vsync::on,
					sge::renderer::no_multi_sampling
				),
				// Copypaste
				sge::systems::viewport::manage_resize()
			)
		)
		(
			// Copypaste
			sge::systems::input(
				sge::systems::input_helper_field(
					sge::systems::input_helper::keyboard_collector) | sge::systems::input_helper::mouse_collector
			)
		)
	);

	// Abkürzung, damit wir nicht immer sys.renderer() schreiben müssen
	sge::renderer::device_ptr const rend(
		sys.renderer());

  /*
	testcase::texture3d mytex(
		vm["directory"].as<fcppt::string>(),
		vm["prefix"].as<fcppt::string>(),
		vm["slice-size"].as<std::size_t>(),
		vm["slice-count"].as<std::size_t>());
  */
  testcase::texture3d mytex(  
    static_cast<std::size_t>( 256 )
    );

	// Unser Shader mit der tollen Klasse sge::shader
	sge::shader::object shader(
		rend,
		// Vertexshader, liegen in unserem Fall im sge-Mediapath
		sgevol::media_path()
			/ FCPPT_TEXT("shaders")
			/ FCPPT_TEXT("vertex.glsl"),
		// Fragmentshader
		sgevol::media_path()
			/ FCPPT_TEXT("shaders")
			/ FCPPT_TEXT("fragment.glsl"),
		// Das hier nimmt sich das Vertexformat (vf::format) und macht das zu einer
		// glsl-Deklaration aller Variablen, d.h. in unserem Fall erstellt das
		// automatisch:
		//
		// in vec3 position;
		//
		// Deshalb sind diese $$$HEADER$$$ in den Shadern. Wenn man jetzt ein
		// Vertexattribut hinzufügt, muss man am Shader nichts mehr ändern!
		sge::shader::vf_to_string<testcase::vf::format>(),
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
        "camera",
				sge::shader::variable_type::uniform,
				sge::renderer::vector3()))
			/*
			(sge::shader::variable(
				"mv",
				sge::shader::variable_type::uniform,
				sge::renderer::matrix4()))*/,
		// sampler (sprich Texturen), die wir im Shader brauchen (ebenfalls in $$$HEADER$$$ drin)
		fcppt::assign::make_container<sge::shader::sampler_sequence>
			(sge::shader::sampler(
				"tex",
				// Selbsterklärend
				rend->create_volume_texture(
					mytex.view(),
					// Lineare Filterung. trilinear und point sind auch möglich (und
					// sogar anisotropisch, aber das ist ungetestet)
					sge::renderer::filter::linear,
					// Hier könnte man eine Textur erstellen, die "readable" ist, wenn
					// man die unbedingt wieder auslesen will
					sge::renderer::resource_flags::none),
				// Man muss bei 3D-Texturen noch angeben, dass die 3 Dimensionen hat.
				// Das kann er aus dem obigen create_volume_texture leider nicht
				// ableiten (ein TODO für Freundlich?)
				3)));

	sge::renderer::vertex_buffer_ptr const vb = 
		testcase::create_cube(
			rend,
			shader);

	bool running = true;

	// Registriert einen Keyboard-Listener, der auf Escape wartet und dann das
	// obige "running" auf false setzt.
	fcppt::signal::scoped_connection const cb(
		sys.keyboard_collector()->key_callback(
			sge::input::keyboard::action(
				sge::input::keyboard::key_code::escape,
				boost::phoenix::ref(running) = false
			)
		)
	);

	// Renderstates!
	rend->state(
		sge::renderer::state::list
			// Bildschirm bei jedem Renderdurchgang neu initialisieren?
			(sge::renderer::state::bool_::clear_backbuffer = true)
			// Z-Buffer auch löschen? Braucht man hier glaub ich nichtmal
			(sge::renderer::state::bool_::clear_zbuffer = true)
			// Alphablending plus die zwei Kombinationsfunktionen
			(sge::renderer::state::bool_::enable_alpha_blending = true)
			(sge::renderer::state::source_blend_func::src_alpha)
			(sge::renderer::state::dest_blend_func::inv_src_alpha)
			// Kein Culling
			(sge::renderer::state::cull_mode::off)
			// WIREFRAME
			// (sge::renderer::state::draw_mode::line)
			// Tiefenfunktion
			(sge::renderer::state::depth_func::less)
			// Mit was soll der Tiefen- und Backbuffer initialisiert werden?
			(sge::renderer::state::float_::zbuffer_clear_val = 1.f)
			(sge::renderer::state::color::clear_color = sge::image::colors::black()));

	// Vertexbuffer aktivieren. Muss man machen
	sge::renderer::scoped_vertex_buffer const vb_context(
		rend,
		vb);

	// Kamera sollte bekannt sein
	sge::camera::object cam(
		sge::camera::parameters(
			// Perspektivische Projektion. projection::orthogonal() wäre auch möglich
			sge::camera::projection::perspective(
				// aspect
				sge::renderer::aspect<sge::renderer::scalar>(
					rend->screen_size()),
				// fov
				fcppt::math::deg_to_rad(
					static_cast<sge::renderer::scalar>(
						90)),
				// near
				static_cast<sge::renderer::scalar>(
					0.1),
				// far
				static_cast<sge::renderer::scalar>(
					10)),
			// movementspeed
			static_cast<sge::renderer::scalar>(
				1.0),
			// mousespeed
			static_cast<sge::renderer::scalar>(
				100.0),
			// position
			sge::renderer::vector3( 1, 2.5, 5 ),
			// Maus und Keyboard
			*sys.keyboard_collector(),
			*sys.mouse_collector()));

	sge::time::timer frame_timer(
		sge::time::second(
			1));

	while(running)
	{
		// Sonst werden keine Input-Events geschickt
		sys.window()->dispatch();

		// Beginne Renderdurchgang
		sge::renderer::scoped_block const block_(rend);

		// Shader aktivieren
		sge::shader::scoped scoped_shader(
			shader);

		// Rendern (copypaste)
		rend->render(
			sge::renderer::first_vertex(0),
			sge::renderer::vertex_count(vb->size()),
			sge::renderer::nonindexed_primitive_type::triangle);

		// Kamera updaten (bewegen je nach Pfeiltasten)
		cam.update(
			static_cast<sge::renderer::scalar>(
				frame_timer.reset()));

		// mvp updaten
		shader.set_uniform(
			"mvp",
			cam.mvp());

    shader.set_uniform(
      "camera",
      cam.gizmo().position());

		shader.set_uniform(
			"mv",
			cam.world());
	}
}
catch(sge::exception const &e)
{
	fcppt::io::cerr << e.string() << FCPPT_TEXT('\n');
	return EXIT_FAILURE;
}
catch(fcppt::exception const &e)
{
	fcppt::io::cerr << e.string() << FCPPT_TEXT("\n");
}
catch(std::exception const &e)
{
	std::cerr << e.what() << FCPPT_TEXT('\n');
	return EXIT_FAILURE;
}
