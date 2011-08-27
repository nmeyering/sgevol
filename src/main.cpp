#include <sge/camera/duration.hpp>
#include <sge/camera/identity_gizmo.hpp>
#include <sge/camera/movement_speed.hpp>
#include <sge/camera/object.hpp>
#include <sge/camera/parameters.hpp>
#include <sge/camera/projection/update_perspective_from_viewport.hpp>
#include <sge/camera/rotation_speed.hpp>
#include <sge/config/media_path.hpp>
#include <sge/console/arg_list.hpp>
#include <sge/console/gfx.hpp>
#include <sge/console/object.hpp>
#include <sge/console/output_line_limit.hpp>
#include <sge/console/sprite_object.hpp>
#include <sge/console/sprite_parameters.hpp>
#include <sge/exception.hpp>
#include <sge/font/metrics_ptr.hpp>
#include <sge/font/rect.hpp>
#include <sge/font/size_type.hpp>
#include <sge/font/system.hpp>
#include <sge/font/text/align_h.hpp>
#include <sge/font/text/flags.hpp>
#include <sge/renderer/resource_flags.hpp>
#include <sge/font/text/align_v.hpp>
#include <sge/font/text/draw.hpp>
#include <sge/font/text/drawer_3d.hpp>
#include <sge/font/text/flags_none.hpp>
#include <sge/font/text/from_fcppt_string.hpp>
#include <sge/font/text/lit.hpp>
#include <sge/font/text/part.hpp>
#include <sge/font/text/string.hpp>
#include <sge/image/color/any/object.hpp>
#include <sge/image/color/init.hpp>
#include <sge/image/color/rgba8.hpp>
#include <sge/image/colors.hpp>
#include <sge/input/keyboard/action.hpp>
#include <sge/input/keyboard/device.hpp>
#include <sge/input/keyboard/key_code.hpp>
#include <sge/parse/json/parse_file_exn.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/config/create_command_line_parameters.hpp>
#include <sge/parse/json/config/merge_command_line_parameters.hpp>
#include <sge/parse/json/object.hpp>
#include <sge/parse/json/path.hpp>
#include <sge/parse/json/value.hpp>
#include <sge/renderer/depth_stencil_buffer.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/first_vertex.hpp>
#include <sge/renderer/no_multi_sampling.hpp>
#include <sge/renderer/nonindexed_primitive_type.hpp>
#include <sge/renderer/onscreen_target.hpp>
#include <sge/renderer/parameters.hpp>
#include <sge/renderer/projection/far.hpp>
#include <sge/renderer/projection/fov.hpp>
#include <sge/renderer/projection/near.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scalar.hpp>
#include <sge/renderer/scoped_block.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/renderer/state/cull_mode.hpp>
#include <sge/renderer/state/depth_func.hpp>
#include <sge/renderer/state/dest_blend_func.hpp>
#include <sge/renderer/state/draw_mode.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/source_blend_func.hpp>
#include <sge/renderer/state/trampoline.hpp>
#include <sge/renderer/texture/address_mode.hpp>
#include <sge/renderer/texture/address_mode3.hpp>
#include <sge/renderer/texture/create_volume_from_view.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/renderer/vector3.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/renderer/vertex_count.hpp>
#include <sge/renderer/vertex_declaration_ptr.hpp>
#include <sge/renderer/visual_depth.hpp>
#include <sge/renderer/vsync.hpp>
#include <sge/shader/activation_method.hpp>
#include <sge/shader/activation_method_field.hpp>
#include <sge/shader/matrix.hpp>
#include <sge/shader/matrix_flags.hpp>
#include <sge/shader/object.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/sprite/object_impl.hpp>
#include <sge/sprite/parameters_impl.hpp>
#include <sge/systems/cursor_option.hpp>
#include <sge/systems/cursor_option_field.hpp>
#include <sge/systems/input.hpp>
#include <sge/systems/input_helper.hpp>
#include <sge/systems/input_helper_field.hpp>
#include <sge/systems/instance.hpp>
#include <sge/systems/list.hpp>
#include <sge/systems/parameterless.hpp>
#include <sge/systems/renderer.hpp>
#include <sge/systems/window.hpp>
#include <sge/texture/part_ptr.hpp>
#include <sge/timer/basic.hpp>
#include <sge/timer/clocks/standard.hpp>
#include <sge/timer/elapsed.hpp>
#include <sge/timer/frames_counter.hpp>
#include <sge/timer/parameters.hpp>
#include <sge/timer/reset_when_expired.hpp>
#include <sge/viewport/fill_on_resize.hpp>
#include <sge/viewport/manager.hpp>
#include <sge/window/dim.hpp>
#include <sge/window/instance.hpp>
#include <sge/window/simple_parameters.hpp>
#include <fcppt/chrono/milliseconds.hpp>
#include <fcppt/chrono/seconds.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/filesystem/path.hpp>
#include <fcppt/format.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/math/box/structure_cast.hpp>
#include <fcppt/math/deg_to_rad.hpp>
#include <fcppt/math/pi.hpp>
#include <fcppt/math/twopi.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/scoped_ptr.hpp>
#include <fcppt/shared_ptr.hpp>
#include <fcppt/signal/scoped_connection.hpp>
#include <fcppt/string.hpp>
#include <fcppt/thread/object.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/home/phoenix/core/reference.hpp>
#include <boost/spirit/home/phoenix/operator/self.hpp>
#include <exception>
#include <iostream>
#include <cstdlib>
#include <boost/function.hpp>
#include "json/parse_color.hpp"
#include "create_cube.hpp"
#include "create_shader.hpp"
#include "media_path.hpp"
#include "texture3d.hpp"
#include "shadow_volume.hpp"
#include "vf.hpp"

namespace
{

void
toggle_active(
	sge::console::gfx &c)
{
	c.active(!c.active());
}

void
try_catch_action(
	boost::function<void()> &f)
{
	try
	{
		f();
	}
	catch(fcppt::exception e)
	{
		std::cerr << e.string() << std::endl;
		std::terminate();
	}
}

}

int
main(int argc, char **argv)
try
{
	sge::parse::json::object const &config_file =
		sge::parse::json::config::merge_command_line_parameters(
			sge::parse::json::parse_file_exn(
				sgevol::media_path()/FCPPT_TEXT("config.json")),
			sge::parse::json::config::create_command_line_parameters(
				argc,
				argv));

	int texture_size =
		sge::parse::json::find_and_convert_member<int>(
			config_file,
			sge::parse::json::path(
				FCPPT_TEXT("texture-size")));
	fcppt::string shader_file =
		sge::parse::json::find_and_convert_member<fcppt::string>(
			config_file,
			sge::parse::json::path(
			FCPPT_TEXT("shader-file")));
	fcppt::string save_path =
		sge::parse::json::find_and_convert_member<fcppt::string>(
			config_file,
			sge::parse::json::path(
			FCPPT_TEXT("save-path")));
	fcppt::string load_path =
		sge::parse::json::find_and_convert_member<fcppt::string>(
			config_file,
			sge::parse::json::path(
			FCPPT_TEXT("load-path")));
	sge::image::color::rgba8 background_color =
		sgevol::json::parse_color<sge::image::color::rgba8>(
			sge::parse::json::find_and_convert_member<sge::parse::json::value>(
				config_file,
			sge::parse::json::path(
				FCPPT_TEXT("background-color"))));
	bool load_texture =
		sge::parse::json::find_and_convert_member<bool>(
			config_file,
			sge::parse::json::path(
			FCPPT_TEXT("load-texture")));
	bool save_texture =
		sge::parse::json::find_and_convert_member<bool>(
			config_file,
			sge::parse::json::path(
			FCPPT_TEXT("save-texture")));
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
		(sge::systems::window(
				sge::window::simple_parameters(
					// Fenstertitel offenbar
					FCPPT_TEXT("sgevol: cloud volume rendering demo"),
					dimensions)))
		(sge::systems::renderer(
				sge::renderer::parameters(
					sge::renderer::visual_depth::depth32,
					sge::renderer::depth_stencil_buffer::d24,
					sge::renderer::vsync::on,
					sge::renderer::no_multi_sampling),
				sge::viewport::fill_on_resize()))
		(sge::systems::input(
				sge::systems::input_helper_field(
					sge::systems::input_helper::keyboard_collector) |
					sge::systems::input_helper::mouse_collector,
				sge::systems::cursor_option_field(
					sge::systems::cursor_option::exclusive)))
		(sge::systems::parameterless::font)
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

	sgevol::texture3d mytex(
		texture_size);
	/*
	sgevol::shadow_volume shadowtex(
		texture_size,
		mytex.const_view());
		*/

	boost::function<void()> tex_action;

	if (load_texture)
		tex_action =
			std::tr1::bind(
				&sgevol::texture3d::load,
				&mytex,
				fcppt::filesystem::path(load_path));
	else
		tex_action =
			std::tr1::bind(
				&sgevol::texture3d::calculate,
				&mytex);

	/*
	tex_action =
		std::tr1::bind(
			&sgevol::shadow_volume::calculate,
			&shadowtex);
	*/

	// shadowtex.calculate();

	fcppt::thread::object load_thread(
		boost::function<void()>(
			std::tr1::bind(
				try_catch_action,
				tex_action)));

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
			(sge::renderer::state::color::back_buffer_clear_color = 
				sge::image::color::any::object(background_color)));
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
			// movementspeed
			sge::camera::movement_speed(
				0.05f),
			// mousespeed
			sge::camera::rotation_speed(
				400.f),
			// Maus und Keyboard
			sys.keyboard_collector(),
			sys.mouse_collector()
			).gizmo(
			sge::camera::identity_gizmo().position(
				sge::renderer::vector3(
					0.0f,
					0.0f,
					-3.0f
					)))
			);

	fcppt::signal::scoped_connection const viewport_connection(
		sys.viewport_manager().manage_callback(
			std::tr1::bind(
				sge::camera::projection::update_perspective_from_viewport,
				fcppt::ref(
					rend),
				fcppt::ref(
					cam),
				sge::renderer::projection::fov(
					fcppt::math::deg_to_rad(
						60.f)),
				sge::renderer::projection::near(
					0.1f),
				// Far plane
				sge::renderer::projection::far(
					1000.f))));

	bool aborted = false;

	fcppt::signal::scoped_connection cb(
		sys.keyboard_collector().key_callback(
			sge::input::keyboard::action(
				sge::input::keyboard::key_code::escape,
				boost::phoenix::ref(aborted) = true
			)
		)
	);

	sge::timer::basic<sge::timer::clocks::standard> accesstimer(
		sge::timer::parameters<sge::timer::clocks::standard>(
			fcppt::chrono::milliseconds(
				100)));

	float p = 0.f;
	while( !aborted )
	{
		if(sge::timer::reset_when_expired(accesstimer))
			if((p = mytex.progress()) >= 99.f)
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
					sge::renderer::texture::mipmap::off(),
					sge::renderer::texture::address_mode3(
						sge::renderer::texture::address_mode::clamp),
					// Hier könnte man eine Textur erstellen, die "readable" ist, wenn
					// man die unbedingt wieder auslesen will
					sge::renderer::resource_flags::none)
					);

	/*
	shader->update_texture( "shadowtex",
				sge::renderer::texture::create_volume_from_view(
					rend,
					shadowtex.const_view(),
					// Lineare Filterung. trilinear und point sind auch möglich (und
					// sogar anisotropisch, aber das ist ungetestet)
					sge::renderer::texture::mipmap::off(),
					sge::renderer::texture::address_mode3(
						sge::renderer::texture::address_mode::clamp),
					// Hier könnte man eine Textur erstellen, die "readable" ist, wenn
					// man die unbedingt wieder auslesen will
					sge::renderer::resource_flags::none)
					);
	*/

	running = true;

	cb.take(
		sys.keyboard_collector().key_callback(
			sge::input::keyboard::action(
				sge::input::keyboard::key_code::escape,
				boost::phoenix::ref(running) = false)));

	sge::timer::basic<sge::timer::clocks::standard> frame_timer(
		sge::timer::parameters<sge::timer::clocks::standard>(
			fcppt::chrono::seconds(
				1)));

	sge::timer::basic<sge::timer::clocks::standard> offset_timer(
		sge::timer::parameters<sge::timer::clocks::standard>(
			fcppt::chrono::milliseconds(
				50)));

	sge::timer::frames_counter fps_counter;

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
			sge::timer::elapsed<sge::camera::duration>(
				frame_timer));

		// Beginne Renderdurchgang
		sge::renderer::scoped_block const block_(rend);

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


		if (sge::timer::reset_when_expired(offset_timer))
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
					sge::renderer::state::cull_mode::back));

			// mvp updaten
			shader->update_uniform(
				"mvp",
				sge::shader::matrix(
				cam.mvp(),
				sge::shader::matrix_flags::projection));

			shader->update_uniform(
				"camera",
				cam.gizmo().position());

			shader->update_uniform(
				"offset",
				offset);

			shader->update_uniform(
				"mv",
				sge::shader::matrix(
				cam.world(),
				sge::shader::matrix_flags::none));
		}

		fps_counter.update();

		if (gfx.active())
			gfx.render();
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
