#include <sge/exception.hpp>
#include <sge/systems/instance.hpp>
#include <sge/systems/list.hpp>
#include <sge/systems/viewport/fill_on_resize.hpp>
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
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/scoped_block.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_lock.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/refresh_rate_dont_care.hpp>
#include <sge/renderer/no_multi_sampling.hpp>
#include <sge/renderer/texture/create_volume_from_view.hpp>
#include <sge/renderer/texture/address_mode3.hpp>
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
#include <sge/camera/identity_gizmo.hpp>
#include <sge/shader/object.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/variable_type.hpp>
#include <sge/renderer/matrix4.hpp>
#include <sge/shader/variable.hpp>
#include <sge/renderer/state/source_blend_func.hpp>
#include <sge/renderer/state/dest_blend_func.hpp>
#include <sge/renderer/onscreen_target.hpp>
#include <sge/renderer/viewport.hpp>
#include <sge/renderer/aspect_from_viewport.hpp>
#include <fcppt/math/box/structure_cast.hpp>
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
#include <sge/camera/projection/update_perspective_from_viewport.hpp>
#include <sge/renderer/aspect.hpp>
#include <sge/renderer/texture/filter/trilinear.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/time/timer.hpp>
#include <sge/time/second.hpp>
#include <sge/time/millisecond.hpp>
#include <sge/image3d/rgba8.hpp>
#include <sge/image/color/rgba8.hpp>
#include <sge/image/color/init.hpp>
#include <sge/image/store.hpp>
#include <mizuiro/image/view.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <sge/image3d/view/to_const.hpp>
#include <sge/image3d/view/object.hpp>
#include <sge/image3d/view/const_object.hpp>
#include <sge/font/metrics.hpp>
#include <sge/font/system.hpp>
#include <sge/font/text/draw.hpp>
#include <sge/font/text/drawer_3d.hpp>
#include <sge/font/text/flags_none.hpp>
#include <sge/font/text/part.hpp>
#include <sge/font/text/lit.hpp>
#include <sge/config/media_path.hpp>
#include <fcppt/make_shared_ptr.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/clamp.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <fcppt/math/deg_to_rad.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/container/raw_vector.hpp>
#include <fcppt/math/dim/input.hpp>
#include <fcppt/math/dim/output.hpp>
#include <fcppt/io/cifstream.hpp>
#include <fcppt/exception.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <streambuf>
#include <cstddef>
#include "perlin3d.hpp"
#include "locked_value.hpp"

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

sge::renderer::vertex_buffer_ptr const
create_cube(
	sge::renderer::device_ptr const,
	sge::shader::object &);

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

	return vb;
}

class texture3d
{
public:
	typedef sge::image3d::rgba8 store;
	typedef store::view_type v;

	explicit
	texture3d(
		std::size_t const dimension);

	sge::image3d::view::const_object const
	view() const;

	void
	calculate();

	float
	progress();
private:
	v::dim_type::value_type dimension_;
	store store_;
	v view_;
	locked_value<float> progress_;
};

texture3d::texture3d(
	v::dim_type::value_type const _dimension
	)
:
	dimension_(
		_dimension
		),
	store_(
		store::dim_type(
			_dimension,
			_dimension,
			_dimension)),
	view_(
		store_.view())
{
	progress_.value(0.0f);
}

sge::image3d::view::const_object const
texture3d::view() const
{
	// Das hier macht aus einer Sammlung von Bytes eine Struktur, nämlich ein
	// mizuiro::view. Das braucht der Renderer, um eine Textur draus zu
	// generieren. Sollte alles selbsterklärend sein (ausnahmsweise)
	return 
		sge::image3d::view::to_const( view_ );
}

float 
texture3d::progress()
{
	return progress_.value();
}

void
texture3d::calculate()
{
	double red = 1.0;
	double green = 1.0;
	double blue = 1.0;
	double alpha = 0.0;
	typedef fcppt::math::vector::static_< float, 3 >::type vec3;
	typedef v::dim_type::value_type dimtype;
	vec3 center(
			static_cast< float >( dimension_ ) * .5f,
			static_cast< float >( dimension_ ) * .5f,
			static_cast< float >( dimension_ ) * .5f);
	sgevol::perlin3d noise( 256 );
	for (dimtype z = 0; z < dimension_; ++z)
	{
		progress_.value( 100.0f * static_cast<float>(z+1) / static_cast<float>(dimension_) );
		for (dimtype y = 0; y < dimension_; ++y)
			for (dimtype x = 0; x < dimension_; ++x)
			{
				vec3 tmp(
					static_cast< float >( x ),
					static_cast< float >( y ),
					static_cast< float >( z ));

				using fcppt::math::clamp;
				alpha = 
					clamp(
						0.25f * noise.sample( 0.20f * tmp ) +
						noise.sample( 0.10f * tmp ) *
						noise.sample( 0.05f * tmp ),
						0.f,
						1.f
					);
				alpha *= 
					clamp(
						1.0f -
						fcppt::math::vector::length( tmp - center ) /
						(0.5f * static_cast<float>(dimension_)),
						0.f,
						1.f
					);
					
				view_[ v::dim_type(x,y,z) ] = 
					sge::image::color::rgba8(
						(sge::image::color::init::red %= red)
						(sge::image::color::init::green %= green)
						(sge::image::color::init::blue %= blue)
						(sge::image::color::init::alpha %= alpha));
			}
	}
}

}

int 
main(int argc, char **argv)
try
{
	std::size_t texture_size = 128;
	if( argc == 2 )
		 texture_size = boost::lexical_cast<std::size_t>(argv[1]);

	// systems::instance ist eine Hilfsklasse, die es einem abnimmt, alle
	// Plugins, die man so braucht, manuell zu laden und zusammenzustecken.
	// Später arbeitet man nur noch mit sys.renderer(), sys.image_loader() usw.
	//
	// Das sind alles shared_ptr, d.h. man muss auf die Methoden mit
	// sys.renderer()->bla zugreifen statt sys.renderer().bla

	sge::window::dim const dimensions(
		1024,
		768
	);
	sge::systems::instance sys(
		sge::systems::list()
		(
			sge::systems::window(
				sge::window::simple_parameters(
					// Fenstertitel offenbar
					FCPPT_TEXT("sge test for 3d textures"),
					dimensions
				)
			)
		)
		(
			sge::systems::renderer(
				sge::renderer::parameters(
					sge::renderer::visual_depth::depth32,
					sge::renderer::depth_stencil_buffer::off,
					sge::renderer::vsync::on,
					sge::renderer::no_multi_sampling
				),
				sge::systems::viewport::fill_on_resize()
			)
		)
		(
			sge::systems::input(
				sge::systems::input_helper_field(
					sge::systems::input_helper::keyboard_collector) | sge::systems::input_helper::mouse_collector,
				sge::systems::cursor_option_field(
					sge::systems::cursor_option::confine)
			)
		)
		(
			sge::systems::parameterless::font
		)
	);

	// Abkürzung, damit wir nicht immer sys.renderer() schreiben müssen
	sge::renderer::device_ptr const rend(
		sys.renderer());

	sge::font::metrics_ptr const metrics(
		sys.font_system()->create_font(
				sge::config::media_path() 
				/ FCPPT_TEXT("fonts") 
				/ FCPPT_TEXT("default.ttf"),
				static_cast<sge::font::size_type>(96)
		)
	);

	sge::font::text::drawer_3d drawer(
			sys.renderer(),
			sge::image::colors::white()
	);
	
	testcase::texture3d mytex(	
		static_cast<std::size_t>( texture_size )
		);

	boost::thread t( boost::bind( &testcase::texture3d::calculate, &mytex) );

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
			//(sge::renderer::state::draw_mode::line)
			// Tiefenfunktion
			(sge::renderer::state::depth_func::off)
			// Mit was soll der Tiefen- und Backbuffer initialisiert werden?
			(sge::renderer::state::float_::zbuffer_clear_val = 1.f)
			(sge::renderer::state::color::clear_color = sge::image::colors::black()));

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
				"offset",
				sge::shader::variable_type::uniform,
				static_cast<sge::renderer::scalar>(
					1.f)))
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
				"tex", sge::renderer::texture::volume_ptr()
				// Selbsterklärend
				// Man muss bei 3D-Texturen noch angeben, dass die 3 Dimensionen hat.
				// Das kann er aus dem obigen create_volume_texture leider nicht
				// ableiten (ein TODO für Freundlich?)
				)));

	sge::time::timer accesstimer(sge::time::millisecond(100));
	float p = 0.f;
	while( true )
	{
		
		if( accesstimer.update_b() )
			if( (p = mytex.progress()) >= 99.f )
				break;

		sys.window()->dispatch();
		
		sge::renderer::scoped_block const block_(rend);
		
		sge::font::text::draw(
			metrics,
			drawer,
			boost::lexical_cast<sge::font::text::string>(
				static_cast<int>(p) 
			) + 
			SGE_FONT_TEXT_LIT("%"),

      fcppt::math::box::structure_cast<sge::font::rect>(
        rend->onscreen_target()->viewport().get()),

      sge::font::text::align_h::center,
      sge::font::text::align_v::center,
      sge::font::text::flags::none

		);
	
	}
	t.join();

	shader.update_texture( "tex",
				sge::renderer::texture::create_volume_from_view(
					rend,
					mytex.view(),
					// Lineare Filterung. trilinear und point sind auch möglich (und
					// sogar anisotropisch, aber das ist ungetestet)
					sge::renderer::texture::filter::trilinear,
					sge::renderer::texture::address_mode3(
						sge::renderer::texture::address_mode::clamp),
					// Hier könnte man eine Textur erstellen, die "readable" ist, wenn
					// man die unbedingt wieder auslesen will
					sge::renderer::resource_flags::none)
					);


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

	// Vertexbuffer aktivieren. Muss man machen
	sge::renderer::scoped_vertex_buffer const vb_context(
		rend,
		vb);

	// Kamera sollte bekannt sein
	sge::camera::object cam(
		sge::camera::parameters(
			// Perspektivische Projektion. projection::orthogonal() wäre auch möglich
			sge::camera::projection::object(),
			// movementspeed
			static_cast<sge::renderer::scalar>(
				1.0),
			// mousespeed
			static_cast<sge::renderer::scalar>(
				200.0),
			// Maus und Keyboard
			sge::camera::gizmo_type().position(
				sge::renderer::vector3(
					1.0f,
					//1.f,
					0.5f,
					//3.f
					3.0f
					)
				).forward(
				sge::renderer::vector3(
					0.f,
					0.f,
					1.f
					)
				).right(
				sge::renderer::vector3(
					1.f,
					0.f,
					0.f
					)
				).up(
				sge::renderer::vector3(
					0.f,
					1.f,
					0.f
					)
				),
			*sys.keyboard_collector(),
			*sys.mouse_collector(),
			sge::camera::activation_state::active
			));

	fcppt::signal::scoped_connection const viewport_connection(
		sys.manage_viewport_callback(
			std::tr1::bind(
				sge::camera::projection::update_perspective_from_viewport,
				std::tr1::placeholders::_1,
				std::tr1::ref(
					cam
				),
				// fov
				static_cast<
					sge::renderer::scalar
				>(
					fcppt::math::deg_to_rad(
						60.
					)
				),
				// near
				static_cast<
					sge::renderer::scalar
				>(
					0.1
				),
				// far
				static_cast<
					sge::renderer::scalar
				>(
					1000.
				)
			)
		)
	);

	sge::time::timer frame_timer(
		sge::time::second(
			1));
	sge::time::timer offset_timer(
		sge::time::millisecond(
			50));

	sge::renderer::scalar offset = 0.0f;
	while(running)
	{
		// Sonst werden keine Input-Events geschickt
		sys.window()->dispatch();

		cam.update(
			static_cast<sge::renderer::scalar>(
				frame_timer.reset()));

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
		

		if(offset_timer.update_b())
			offset += fcppt::math::pi<float>()/100.f;
	
		if(
				std::abs( cam.gizmo().position().x() ) >= 1.0f ||
				std::abs( cam.gizmo().position().y() ) >= 1.0f ||
				std::abs( cam.gizmo().position().z() ) >= 1.0f
			)
		{
			rend->state(
				sge::renderer::state::list( 
					sge::renderer::state::cull_mode::front
				));
		}
		else
		{
			rend->state(
				sge::renderer::state::list( 
					sge::renderer::state::cull_mode::back
				));
		}

		// mvp updaten
		shader.set_uniform(
			"mvp",
			cam.mvp());

		shader.set_uniform(
			"camera",
			cam.gizmo().position());

		shader.set_uniform(
			"offset",
			std::sin(
				offset));

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
