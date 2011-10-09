#include <sge/all_extensions.hpp>
#include <sge/camera/first_person/object.hpp>
#include <sge/camera/first_person/movement_speed.hpp>
#include <sge/camera/first_person/rotation_speed.hpp>
#include <sge/camera/first_person/parameters.hpp>
#include <sge/camera/identity_gizmo.hpp>
#include <sge/camera/projection/object.hpp>
#include <sge/camera/projection/update_perspective_from_viewport.hpp>
#include <sge/config/media_path.hpp>
#include <sge/console/arg_list.hpp>
#include <sge/console/gfx.hpp>
#include <sge/console/object.hpp>
#include <sge/console/output_line_limit.hpp>
#include <sge/console/sprite_object.hpp>
#include <sge/console/callback/from_functor.hpp>
#include <sge/console/callback/name.hpp>
#include <sge/console/callback/short_description.hpp>
#include <sge/console/sprite_parameters.hpp>
#include <sge/exception.hpp>
#include <sge/font/metrics_ptr.hpp>
#include <sge/font/rect.hpp>
#include <sge/font/size_type.hpp>
#include <sge/font/system.hpp>
#include <sge/font/text/align_h.hpp>
#include <sge/font/text/align_v.hpp>
#include <sge/font/text/draw.hpp>
#include <sge/font/text/drawer_3d.hpp>
#include <sge/font/text/flags.hpp>
#include <sge/font/text/flags_none.hpp>
#include <sge/font/text/from_fcppt_string.hpp>
#include <sge/font/text/lit.hpp>
#include <sge/font/text/part.hpp>
#include <sge/font/text/string.hpp>
#include <sge/image/capabilities_field.hpp>
#include <sge/image/color/any/object.hpp>
#include <sge/image/color/init.hpp>
#include <sge/image/color/rgba8.hpp>
#include <sge/image/colors.hpp>
#include <sge/input/keyboard/action.hpp>
#include <sge/input/keyboard/device.hpp>
#include <sge/input/keyboard/key_code.hpp>
#include <sge/model/obj/create.hpp>
#include <sge/model/obj/loader.hpp>
#include <sge/parse/json/config/create_command_line_parameters.hpp>
#include <sge/parse/json/config/merge_command_line_parameters.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/object.hpp>
#include <sge/parse/json/parse_file_exn.hpp>
#include <sge/parse/json/path.hpp>
#include <sge/parse/json/value.hpp>
#include <sge/renderer/depth_stencil_buffer.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/no_multi_sampling.hpp>
#include <sge/renderer/nonindexed_primitive_type.hpp>
#include <sge/renderer/onscreen_target.hpp>
#include <sge/renderer/parameters.hpp>
#include <sge/renderer/projection/far.hpp>
#include <sge/renderer/projection/fov.hpp>
#include <sge/renderer/projection/near.hpp>
#include <sge/renderer/resource_flags.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scalar.hpp>
#include <sge/renderer/scoped_block.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/renderer/state/depth_func.hpp>
#include <sge/renderer/state/dest_blend_func.hpp>
#include <sge/renderer/state/draw_mode.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/source_blend_func.hpp>
#include <sge/renderer/state/trampoline.hpp>
#include <sge/renderer/texture/create_planar_from_path.hpp>
#include <sge/renderer/texture/address_mode.hpp>
#include <sge/renderer/texture/address_mode2.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/renderer/texture/filter/point.hpp>
#include <sge/renderer/texture/filter/scoped.hpp>
#include <sge/renderer/vector3.hpp>
#include <sge/renderer/visual_depth.hpp>
#include <sge/renderer/vsync.hpp>
#include <sge/sprite/object_impl.hpp>
#include <sge/sprite/parameters_impl.hpp>
#include <sge/systems/cursor_option.hpp>
#include <sge/systems/cursor_option_field.hpp>
#include <sge/systems/image_loader.hpp>
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
#include <fcppt/extract_from_string_exn.hpp>
#include <fcppt/insert_to_string.hpp>
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
#include <boost/spirit/home/phoenix/core/reference.hpp>
#include <boost/spirit/home/phoenix/operator/self.hpp>
#include <exception>
#include <iostream>
#include <cstdlib>
#include <boost/function.hpp>
#include <sgevol/json/parse_color.hpp>
#include <sgevol/media_path.hpp>
#include <sgevol/texture3d.hpp>
#include <sgevol/cloud_sphere/object.hpp>
#include <sgevol/cube/object.hpp>
#include <sgevol/model/object.hpp>
#include <sgevol/noise_volume.hpp>
#include <sgevol/stars/object.hpp>

namespace
{

void
toggle_console(
	sge::console::gfx &console,
	sge::camera::first_person::object &camera)
{
	bool const act = console.active();
	console.active(!act);
	camera.active(act);
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
		fcppt::io::cerr() << e.string() << std::endl;
		std::terminate();
	}
}

void
quit(
	bool &_value)
{
	_value = false;
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

	std::size_t texture_size =
		sge::parse::json::find_and_convert_member<std::size_t>(
			config_file,
			sge::parse::json::path(
				FCPPT_TEXT("texture-size")));
	std::size_t noise_size =
		sge::parse::json::find_and_convert_member<std::size_t>(
			config_file,
			sge::parse::json::path(
				FCPPT_TEXT("noise-size")));
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
	fcppt::string globe_tex_path =
		sge::parse::json::find_and_convert_member<fcppt::string>(
			config_file,
			sge::parse::json::path(
			FCPPT_TEXT("globe-texture")));
	sge::renderer::scalar globe_radius =
		sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
			config_file,
			sge::parse::json::path(
				FCPPT_TEXT("globe-radius")));
	sge::renderer::scalar opacity_factor =
		sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
			config_file,
			sge::parse::json::path(
				FCPPT_TEXT("opacity-factor")));
	sge::image::color::rgba8 background_color =
		sgevol::json::parse_color<sge::image::color::rgba8>(
			sge::parse::json::find_and_convert_member<sge::parse::json::value>(
				config_file,
			sge::parse::json::path(
				FCPPT_TEXT("background-color"))));
	sge::renderer::size_type star_count =
		sge::parse::json::find_and_convert_member<sge::renderer::size_type>(
				config_file,
			sge::parse::json::path(
				FCPPT_TEXT("star-count")));
	sge::renderer::scalar star_size =
		sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				config_file,
			sge::parse::json::path(
				FCPPT_TEXT("star-size")));
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
		(sge::systems::image_loader(
			sge::image::capabilities_field::null(),
			sge::all_extensions))
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

	sgevol::noise_volume noise(
		noise_size);

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

	fcppt::thread::object load_thread(
		boost::function<void()>(
			std::tr1::bind(
				try_catch_action,
				tex_action)));

	std::cout << "load thread started" << std::endl;

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

	// Kamera sollte bekannt sein
	sge::camera::first_person::object cam(
		sge::camera::first_person::parameters(
			// movementspeed
			sge::camera::first_person::movement_speed(
				0.05f),
			// mousespeed
			sge::camera::first_person::rotation_speed(
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
	std::cout << "timer initialized" << std::endl;

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
			fcppt::insert_to_string<sge::font::text::string>(
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

	sge::renderer::texture::planar_ptr globe_tex(
		sge::renderer::texture::create_planar_from_path(
			sgevol::media_path()
				/ FCPPT_TEXT("textures")
				/ globe_tex_path,
				rend,
				sys.image_loader(),
				sge::renderer::texture::mipmap::off(),
					sge::renderer::texture::address_mode2(
						sge::renderer::texture::address_mode::repeat),
					sge::renderer::resource_flags::none
				));

	sge::model::obj::loader_ptr model_loader(
		sge::model::obj::create());
	sge::model::obj::instance_ptr globe_model(
		model_loader->load(
			sgevol::media_path()
				/ FCPPT_TEXT("models")
				/ FCPPT_TEXT("globe.obj")));

	sgevol::model::object globe(
		rend,
		globe_model,
		sgevol::media_path()
			/ FCPPT_TEXT("shaders")
			/ FCPPT_TEXT("vertex")
			/ FCPPT_TEXT("globe.glsl"),
		sgevol::media_path()
			/ FCPPT_TEXT("shaders")
			/ FCPPT_TEXT("fragment")
			/ FCPPT_TEXT("tex_plain.glsl"),
		globe_tex,
		globe_radius,
		cam);

	sgevol::cloud_sphere::object sphere(
		rend,
		globe_model,
		sgevol::media_path()
			/ FCPPT_TEXT("shaders")
			/ FCPPT_TEXT("vertex")
			/ FCPPT_TEXT("vertex.glsl"),
		sgevol::media_path()
			/ FCPPT_TEXT("shaders")
			/ FCPPT_TEXT("fragment")
			/ (fcppt::format(FCPPT_TEXT("%1%.glsl")) % shader_file).str(),
		cam,
		globe_radius,
		opacity_factor,
		mytex.const_view(),
		noise.const_view());


	sgevol::stars::object stars(
		star_count,
		star_size,
		rend,
		sgevol::media_path()
			/ FCPPT_TEXT("shaders")
			/ FCPPT_TEXT("vertex")
			/ FCPPT_TEXT("stars.glsl"),
		sgevol::media_path()
			/ FCPPT_TEXT("shaders")
			/ FCPPT_TEXT("fragment")
			/ FCPPT_TEXT("stars.glsl"),
		cam);

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

	// console begin
	sge::console::object console(
		SGE_FONT_TEXT_LIT('/')
	);

	fcppt::signal::scoped_connection const quit_conn(
		console.insert(
			sge::console::callback::from_functor<void()>(
				std::tr1::bind(
					&quit,
					fcppt::ref(
						running)),
				sge::console::callback::name(
					SGE_FONT_TEXT_LIT("quit")),
				sge::console::callback::short_description(
					SGE_FONT_TEXT_LIT("quit demo")))));

	fcppt::signal::scoped_connection const opacity_conn(
		console.insert(
			sge::console::callback::from_functor<void(sge::renderer::scalar)>(
				std::tr1::bind(
					static_cast<void(sgevol::cloud_sphere::object::*)(sge::renderer::scalar)>(&sgevol::cloud_sphere::object::opacity),
					&sphere,
					std::tr1::placeholders::_1),
				sge::console::callback::name(
					SGE_FONT_TEXT_LIT("opacity")),
				sge::console::callback::short_description(
					SGE_FONT_TEXT_LIT("cloud opacity")))));

	sge::font::metrics_ptr const console_metrics(
		sys.font_system().create_font(
				sge::config::media_path()
				/ FCPPT_TEXT("fonts")
				/ FCPPT_TEXT("default.ttf"),
				static_cast<sge::font::size_type>(16)
		)
	);

	sge::console::gfx console_gfx(
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
					&toggle_console,
					fcppt::ref(
						console_gfx),
					fcppt::ref(
						cam)))));

	console_gfx.active(false);

	// console end

	while(running)
	{
		// Sonst werden keine Input-Events geschickt
		sys.window().dispatch();

		cam.update(
			sge::timer::elapsed<sge::camera::duration>(
				frame_timer));

		// Beginne Renderdurchgang
		sge::renderer::scoped_block const block_(rend);

		stars.render();

		/*
		{
			sge::renderer::texture::filter::scoped const scoped_filter(
				rend,
				sge::renderer::stage(0u),
				sge::renderer::texture::filter::point()
			);

			cube.render();
		}
		*/

		globe.render();
		sphere.render();

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

		fps_counter.update();

		if (console_gfx.active())
			console_gfx.render();
	}
}
catch(sge::exception const &e)
{
	fcppt::io::cerr() << e.string() << FCPPT_TEXT('\n');
	return EXIT_FAILURE;
}
catch(fcppt::exception const &e)
{
	fcppt::io::cerr() << e.string() << FCPPT_TEXT("\n");
}
catch(std::exception const &e)
{
	std::cerr << e.what() << FCPPT_TEXT('\n');
	return EXIT_FAILURE;
}
