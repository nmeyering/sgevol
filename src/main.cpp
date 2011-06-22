#include <sge/camera/identity_gizmo.hpp>
#include <sge/camera/object.hpp>
#include <sge/camera/parameters.hpp>
#include <sge/camera/projection/perspective.hpp>
#include <sge/camera/projection/update_perspective_from_viewport.hpp>
#include <sge/config/media_path.hpp>
#include <sge/console/console.hpp>
#include <sge/exception.hpp>
#include <sge/font/font.hpp>
#include <sge/image/color/color.hpp>
#include <sge/image/colors.hpp>
#include <sge/input/keyboard/action.hpp>
#include <sge/input/keyboard/device.hpp>
#include <sge/parse/json/json.hpp>
#include <sge/renderer/renderer.hpp>
#include <sge/shader/activate_everything.hpp>
#include <sge/shader/shader.hpp>
#include <sge/sprite/parameters.hpp>
#include <sge/sprite/object_impl.hpp>
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
#include <fcppt/format.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/io/cifstream.hpp>
#include <fcppt/math/box/structure_cast.hpp>
#include <fcppt/math/deg_to_rad.hpp>
#include <fcppt/math/twopi.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/thread/thread.hpp>
#include <fcppt/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/home/phoenix/core/reference.hpp>
#include <boost/spirit/home/phoenix/operator/self.hpp>
#include <exception>
#include <iostream>
#include <cstdlib>
#include "json/find_member.hpp"
#include "json/parse_color.hpp"
#include "config_wrapper.hpp"
#include "create_cube.hpp"
#include "create_shader.hpp"
#include "media_path.hpp"
#include "texture3d.hpp"
#include "vf.hpp"

using sgevol::texture3d;

namespace
{

void
toggle_active(
	sge::console::gfx &c)
{
	c.active(!c.active());
}

}

int 
main(int argc, char **argv)
try
{
	sge::parse::json::object config_file(
		sgevol::config_wrapper(
			argc,
			argv));

	std::size_t texture_size =
		sgevol::json::find_member<std::size_t>(
			config_file,
			FCPPT_TEXT("texture-size"));
	fcppt::string shader_file = 
		sgevol::json::find_member<fcppt::string>(
			config_file,
			FCPPT_TEXT("shader-file"));
	fcppt::string save_path = 
		sgevol::json::find_member<fcppt::string>(
			config_file,
			FCPPT_TEXT("save-path"));
	fcppt::string load_path = 
		sgevol::json::find_member<fcppt::string>(
			config_file,
			FCPPT_TEXT("load-path"));
	sge::image::color::rgba8 background_color =
		sgevol::json::parse_color<sge::image::color::rgba8>(
			sgevol::json::find_member<sge::parse::json::value>(
				config_file,
				FCPPT_TEXT("background-color")));
	bool load_texture =
		sgevol::json::find_member<bool>(
			config_file,
			FCPPT_TEXT("load-texture"));
	bool save_texture =
		sgevol::json::find_member<bool>(
			config_file,
			FCPPT_TEXT("save-texture"));
	// nonsensical...
	if (load_texture)
		save_texture = false;

	bool running = false;
	

	// systems::instance ist eine Hilfsklasse, die es einem abnimmt, alle
	// Plugins, die man so braucht, manuell zu laden und zusammenzustecken.
	// Später arbeitet man nur noch mit sys.renderer(), sys.image_loader() usw.

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
					sge::systems::input_helper::keyboard_collector) |
					sge::systems::input_helper::mouse_collector,
				sge::systems::cursor_option_field(
					sge::systems::cursor_option::exclusive)
			)
		)
		(
			sge::systems::parameterless::font
		)
	);

	// Abkürzung, damit wir nicht immer sys.renderer() schreiben müssen
	sge::renderer::device& rend(
		sys.renderer());

	// console begin
	sge::console::object console(
		SGE_FONT_TEXT_LIT('/')
	);

	fcppt::signal::scoped_connection const quit_conn(
		console.insert(
			SGE_FONT_TEXT_LIT("quit"),
			boost::phoenix::ref(running) = false,
			SGE_FONT_TEXT_LIT("quit demo")
		)
	);

	sge::font::metrics_ptr const console_metrics(
		sys.font_system().create_font(
				sge::config::media_path() 
				/ FCPPT_TEXT("fonts") 
				/ FCPPT_TEXT("default.ttf"),
				static_cast<sge::font::size_type>(16)
		)
	);

	sge::console::gfx gfx(
		console,
		sys.renderer(),
		sge::image::colors::black(),
		*console_metrics,
		sys.keyboard_collector(),
		sge::console::sprite_object(
			sge::console::sprite_parameters()
			.pos(
				sge::console::sprite_object::vector::null()
			)
			.texture(
				sge::texture::part_ptr()
			)
			.size(
				sge::console::sprite_object::dim(400,300)
			)
			.elements()
		),
		static_cast<
			sge::console::output_line_limit
		>(
			100
		)
	);

	fcppt::signal::scoped_connection console_cb(
		sys.keyboard_collector().key_callback(
			sge::input::keyboard::action(
				sge::input::keyboard::key_code::f1,
				std::tr1::bind(
					&toggle_active,
					fcppt::ref(
						gfx)))));

	gfx.active(false);

	// console end

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
	
	// typedef fcppt::scoped_ptr<texture3d> texture_scoped_ptr;

	texture3d mytex(
		texture_size);

	boost::function<void()> tex_action;

	if (load_texture)
		tex_action =
			std::tr1::bind(
				&texture3d::load,
				&mytex,
				fcppt::filesystem::path(load_path));
	else
		tex_action =
			std::tr1::bind(
				&texture3d::calculate,
				&mytex);

	fcppt::thread::object load_thread(
		tex_action);

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
			(sge::renderer::state::color::back_buffer_clear_color = background_color));
			/*
			sge::image::color::rgba8(
				(sge::image::color::init::red %= 0.5)
				(sge::image::color::init::green %= 0.5)
				(sge::image::color::init::blue %= 1.0)
				(sge::image::color::init::alpha %= 0.5)
				)));
			*/

	std::pair<sge::renderer::vertex_buffer_ptr,sge::renderer::vertex_declaration_ptr> const buffer_and_declaration =
		sgevol::create_cube(
			rend);

	fcppt::shared_ptr<sge::shader::object> shader =
	sgevol::create_shader(
		rend,
		buffer_and_declaration.second,
		sgevol::media_path()
			/ FCPPT_TEXT("shaders")
			/ FCPPT_TEXT("fragment")
			/ (fcppt::format(FCPPT_TEXT("%1%.glsl")) % shader_file).str());

	// Kamera sollte bekannt sein
	sge::camera::object cam(
		sge::camera::parameters(
			// Perspektivische Projektion. projection::orthogonal() wäre auch möglich
			sge::camera::projection::object(),
			// movementspeed
			static_cast<sge::renderer::scalar>(
				2.0),
			// mousespeed
			static_cast<sge::renderer::scalar>(
				400.0),
			// Maus und Keyboard
			sge::camera::identity_gizmo().position(
				sge::renderer::vector3(
					0.0f,
					0.0f,
					3.0f
					)),
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

	bool aborted = false;

	fcppt::signal::scoped_connection cb(
		sys.keyboard_collector().key_callback(
			sge::input::keyboard::action(
				sge::input::keyboard::key_code::escape,
				boost::phoenix::ref(aborted) = true
			)
		)
	);

	sge::time::timer accesstimer(sge::time::millisecond(100));
	float p = 0.f;
	while( !aborted )
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
	if (aborted)
		return EXIT_FAILURE;

	load_thread.join();

	if (save_texture)
		mytex.save(
			fcppt::filesystem::path(
			save_path));

	shader->update_texture( "tex",
				sge::renderer::texture::create_volume_from_view(
					rend,
					mytex.const_view(),
					// Lineare Filterung. trilinear und point sind auch möglich (und
					// sogar anisotropisch, aber das ist ungetestet)
					sge::renderer::texture::filter::trilinear,
					sge::renderer::texture::address_mode3(
						sge::renderer::texture::address_mode::clamp),
					// Hier könnte man eine Textur erstellen, die "readable" ist, wenn
					// man die unbedingt wieder auslesen will
					sge::renderer::resource_flags::none)
					);

	running = true;

	cb.take(
		sys.keyboard_collector().key_callback(
			sge::input::keyboard::action(
				sge::input::keyboard::key_code::escape,
				boost::phoenix::ref(running) = false)));

	sge::time::timer frame_timer(
		sge::time::second(
			1));
	sge::time::timer offset_timer(
		sge::time::millisecond(
			50));
	sge::time::frames_counter fps_counter;

	sge::renderer::scalar offset = 0.0f;

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
			*shader,
			sge::shader::activation_method_field(
				sge::shader::activation_method::textures));

		// Vertexbuffer aktivieren. Muss man machen
		sge::renderer::scoped_vertex_declaration const decl_context(
			rend,
			*buffer_and_declaration.second);

		sge::renderer::scoped_vertex_buffer const vb_context(
			rend,
			*buffer_and_declaration.first);

		// Rendern (copypaste)
		rend.render_nonindexed(
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
				"mv",
				cam.world());
		}

		fps_counter.update();

		if (gfx.active())
			gfx.draw();

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
