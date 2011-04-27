#include <sge/camera/identity_gizmo.hpp>
#include <sge/camera/object.hpp>
#include <sge/camera/parameters.hpp>
#include <sge/camera/projection/perspective.hpp>
#include <sge/camera/projection/update_perspective_from_viewport.hpp>
#include <sge/config/media_path.hpp>
#include <sge/exception.hpp>
#include <sge/font/metrics.hpp>
#include <sge/font/system.hpp>
#include <sge/font/text/draw.hpp>
#include <sge/font/text/drawer_3d.hpp>
#include <sge/font/text/flags_none.hpp>
#include <sge/font/text/from_fcppt_string.hpp>
#include <sge/font/text/lit.hpp>
#include <sge/font/text/part.hpp>
#include <sge/image/colors.hpp>
#include <sge/input/keyboard/action.hpp>
#include <sge/input/keyboard/device.hpp>
#include <sge/renderer/aspect.hpp>
#include <sge/renderer/aspect_from_viewport.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/matrix4.hpp>
#include <sge/renderer/no_multi_sampling.hpp>
#include <sge/renderer/onscreen_target.hpp>
#include <sge/renderer/refresh_rate_dont_care.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scoped_block.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/state/cull_mode.hpp>
#include <sge/renderer/state/depth_func.hpp>
#include <sge/renderer/state/dest_blend_func.hpp>
#include <sge/renderer/state/float.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/source_blend_func.hpp>
#include <sge/renderer/state/trampoline.hpp>
#include <sge/renderer/state/var.hpp>
#include <sge/renderer/texture/address_mode3.hpp>
#include <sge/renderer/texture/create_volume_from_view.hpp>
#include <sge/renderer/texture/filter/trilinear.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vertex_declaration_ptr.hpp>
#include <sge/renderer/viewport.hpp>
#include <sge/shader/object.hpp>
#include <sge/shader/sampler.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/variable.hpp>
#include <sge/shader/variable_type.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <sge/systems/instance.hpp>
#include <sge/systems/list.hpp>
#include <sge/time/frames_counter.hpp>
#include <sge/time/millisecond.hpp>
#include <sge/time/second.hpp>
#include <sge/time/timer.hpp>
#include <sge/viewport/fill_on_resize.hpp>
#include <sge/viewport/manager.hpp>
#include <sge/window/instance.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/io/cifstream.hpp>
#include <fcppt/math/box/structure_cast.hpp>
#include <fcppt/math/deg_to_rad.hpp>
#include <fcppt/ref.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/home/phoenix/core/reference.hpp>
#include <boost/spirit/home/phoenix/operator/self.hpp>
#include <boost/thread.hpp>
#include <exception>
#include <iostream>
#include "create_cube.hpp"
#include "media_path.hpp"
#include "texture3d.hpp"
#include "vf.hpp"

using sgevol::texture3d;
using sgevol::create_cube;

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
				sge::viewport::fill_on_resize()
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
			rend,
			sge::image::colors::white()
	);
	
	texture3d mytex(
		static_cast<std::size_t>( texture_size )
		);

	boost::thread t( boost::bind( &texture3d::calculate, &mytex) );

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
			/ FCPPT_TEXT("vertex")
			/ FCPPT_TEXT("vertex.glsl"),
		// Fragmentshader
		sgevol::media_path()
			/ FCPPT_TEXT("shaders")
			/ FCPPT_TEXT("fragment")
			/ FCPPT_TEXT("fragment.glsl"),
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
		sys.viewport_manager().manage_callback(
			std::tr1::bind(
				sge::camera::projection::update_perspective_from_viewport,
				fcppt::ref(
					*rend),
				fcppt::ref(
					cam),
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

	std::pair<sge::renderer::vertex_buffer_ptr,sge::renderer::vertex_declaration_ptr> const buffer_and_declaration =
		create_cube(
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

	sge::time::timer frame_timer(
		sge::time::second(
			1));
	sge::time::timer offset_timer(
		sge::time::millisecond(
			50));
	sge::time::frames_counter fps_counter;

	sge::renderer::scalar offset = 0.0f;

	sge::font::metrics_ptr const fps_metrics(
		sys.font_system()->create_font(
				sge::config::media_path() 
				/ FCPPT_TEXT("fonts") 
				/ FCPPT_TEXT("default.ttf"),
				static_cast<sge::font::size_type>(32)
		)
	);

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
		{
		sge::shader::scoped scoped_shader(
			shader);

		// Vertexbuffer aktivieren. Muss man machen
		sge::renderer::scoped_vertex_declaration const decl_context(
			rend,
			buffer_and_declaration.second);

		sge::renderer::scoped_vertex_buffer const vb_context(
			rend,
			buffer_and_declaration.first);

		// Rendern (copypaste)
		rend->render(
			sge::renderer::first_vertex(0),
			sge::renderer::vertex_count(buffer_and_declaration.first->size()),
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
			shader.update_uniform(
				"mvp",
				cam.mvp());

			shader.update_uniform(
				"camera",
				cam.gizmo().position());

			shader.update_uniform(
				"offset",
				std::sin(
					offset));

			shader.update_uniform(
				"mv",
				cam.world());
		}

		fps_counter.update();

		sge::font::text::draw(
			fps_metrics,
			drawer,
			sge::font::text::from_fcppt_string(
				fps_counter.frames_str())
			+ SGE_FONT_TEXT_LIT(" fps"),
			fcppt::math::box::structure_cast<sge::font::rect>(
				rend->onscreen_target()->viewport().get()),
			sge::font::text::align_h::left,
			sge::font::text::align_v::top,
			sge::font::text::flags::none
		);

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
