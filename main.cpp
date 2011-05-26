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
#include <sge/image/color/color.hpp>
#include <sge/image/colors.hpp>
#include <sge/input/keyboard/action.hpp>
#include <sge/input/keyboard/device.hpp>
#include <sge/renderer/renderer.hpp>
#include <sge/shader/shader.hpp>
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
#include <fcppt/math/twopi.hpp>
#include <fcppt/ref.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/home/phoenix/core/reference.hpp>
#include <boost/spirit/home/phoenix/operator/self.hpp>
#include <boost/thread.hpp>
#include <exception>
#include <iostream>
#include "create_cube.hpp"
#include "create_shader.hpp"
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
	sge::renderer::device& rend(
		sys.renderer());

	sge::font::metrics_ptr const metrics(
		sys.font_system().create_font(
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
	rend.state(
		sge::renderer::state::list
			// Bildschirm bei jedem Renderdurchgang neu initialisieren?
			(sge::renderer::state::bool_::clear_back_buffer = true)
			// Z-Buffer auch löschen? Braucht man hier glaub ich nichtmal
			(sge::renderer::state::bool_::clear_depth_buffer = true)
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
			(sge::renderer::state::float_::depth_buffer_clear_val = 1.f)
			(sge::renderer::state::color::back_buffer_clear_color = sge::image::color::rgba8(
				(sge::image::color::init::red %= 0.5)
				(sge::image::color::init::green %= 0.5)
				(sge::image::color::init::blue %= 1.0)
				(sge::image::color::init::alpha %= 1.0)
				)));

	fcppt::shared_ptr<sge::shader::object> shader =
	sgevol::create_shader(
		rend,
		sgevol::media_path()
			/ FCPPT_TEXT("shaders")
			/ FCPPT_TEXT("fragment")
			/ FCPPT_TEXT("local"));

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
				400.0),
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
			sys.keyboard_collector(),
			sys.mouse_collector(),
			sge::camera::activation_state::active
			));

	fcppt::signal::scoped_connection const viewport_connection(
		sys.viewport_manager().manage_callback(
			std::tr1::bind(
				sge::camera::projection::update_perspective_from_viewport,
				fcppt::ref(
					rend),
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

		sys.window().dispatch();
		
		sge::renderer::scoped_block const block_(rend);
		
		sge::font::text::draw(
			*metrics,
			drawer,
			boost::lexical_cast<sge::font::text::string>(
				static_cast<int>(p) 
			) + 
			SGE_FONT_TEXT_LIT("%"),
			fcppt::math::box::structure_cast<sge::font::rect>(
				rend.onscreen_target().viewport().get()),
			sge::font::text::align_h::center,
			sge::font::text::align_v::center,
			sge::font::text::flags::none
		);
	
	}
	t.join();

	shader->update_texture( "tex",
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
			*shader);

	bool running = true;

	// Registriert einen Keyboard-Listener, der auf Escape wartet und dann das
	// obige "running" auf false setzt.
	fcppt::signal::scoped_connection const cb(
		sys.keyboard_collector().key_callback(
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
	sge::time::timer light_timer(
		sge::time::second(
			5));
	sge::time::frames_counter fps_counter;

	sge::renderer::scalar offset = 0.0f;
	sge::renderer::scalar light = 0.0f;

	sge::font::metrics_ptr const fps_metrics(
		sys.font_system().create_font(
				sge::config::media_path() 
				/ FCPPT_TEXT("fonts") 
				/ FCPPT_TEXT("default.ttf"),
				static_cast<sge::font::size_type>(32)
		)
	);

	while(running)
	{
		// Sonst werden keine Input-Events geschickt
		sys.window().dispatch();

		cam.update(
			static_cast<sge::renderer::scalar>(
				frame_timer.reset()));

		// Beginne Renderdurchgang
		sge::renderer::scoped_block const block_(rend);

		// Shader aktivieren
		{
		sge::shader::scoped scoped_shader(
			*shader);

		// Vertexbuffer aktivieren. Muss man machen
		sge::renderer::scoped_vertex_declaration const decl_context(
			rend,
			*buffer_and_declaration.second);

		sge::renderer::scoped_vertex_buffer const vb_context(
			rend,
			*buffer_and_declaration.first);

		if (light_timer.update_b())
			light = (light > 0.5f) ? 0.0f : 1.0f;

		// Rendern (copypaste)
		rend.render(
			sge::renderer::first_vertex(0),
			sge::renderer::vertex_count(buffer_and_declaration.first->size()),
			sge::renderer::nonindexed_primitive_type::triangle);
			if (offset_timer.update_b())
				offset += fcppt::math::pi<float>()/50.f;
				if (offset > fcppt::math::twopi<float>())
					offset = 0.f;
		
			/*
			if(
					std::abs( cam.gizmo().position().x() ) >= 1.0f ||
					std::abs( cam.gizmo().position().y() ) >= 1.0f ||
					std::abs( cam.gizmo().position().z() ) >= 1.0f
				)
			{
				rend.state(
					sge::renderer::state::list( 
						sge::renderer::state::cull_mode::front
					));
			}
			else
			{
				rend.state(
					sge::renderer::state::list( 
						sge::renderer::state::cull_mode::back
					));
			}
			*/
			rend.state(
				sge::renderer::state::list(
					sge::renderer::state::cull_mode::front));

			// mvp updaten
			shader->update_uniform(
				"mvp",
				cam.mvp());

			shader->update_uniform(
				"camera",
				cam.gizmo().position());

			shader->update_uniform(
				"offset",
				offset);

			shader->update_uniform(
				"light",
				light);

			shader->update_uniform(
				"mv",
				cam.world());
		}

		fps_counter.update();

		sge::font::text::draw(
			*fps_metrics,
			drawer,
			sge::font::text::from_fcppt_string(
				fps_counter.frames_str())
			+ SGE_FONT_TEXT_LIT(" fps"),
			fcppt::math::box::structure_cast<sge::font::rect>(
				rend.onscreen_target().viewport().get()),
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
