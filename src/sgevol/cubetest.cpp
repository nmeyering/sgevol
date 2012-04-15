#include <sgevollib/media_path.hpp>
#include <sgevollib/noise_volume.hpp>
#include <sgevollib/texture3d.hpp>
#include <sgevollib/cube/object.hpp>
#include <sgevollib/json/parse_color.hpp>
#include <sgevollib/stars/object.hpp>
#include <awl/main/exit_code.hpp>
#include <awl/main/exit_failure.hpp>
#include <awl/main/exit_success.hpp>
#include <awl/main/function_context.hpp>
#include <sge/exception.hpp>
#include <sge/camera/base.hpp>
#include <sge/camera/has_activation.hpp>
#include <sge/camera/perspective_projection_from_viewport.hpp>
#include <sge/camera/update_duration.hpp>
#include <sge/camera/coordinate_system/forward.hpp>
#include <sge/camera/coordinate_system/object.hpp>
#include <sge/camera/coordinate_system/position.hpp>
#include <sge/camera/coordinate_system/right.hpp>
#include <sge/camera/coordinate_system/up.hpp>
#include <sge/camera/first_person/is_active.hpp>
#include <sge/camera/first_person/mouse_speed_multiplier.hpp>
#include <sge/camera/first_person/movement_speed.hpp>
#include <sge/camera/first_person/object.hpp>
#include <sge/camera/first_person/parameters.hpp>
#include <sge/camera/spherical/acceleration_factor.hpp>
#include <sge/camera/spherical/damping_factor.hpp>
#include <sge/camera/spherical/is_active.hpp>
#include <sge/camera/spherical/maximum_radius.hpp>
#include <sge/camera/spherical/minimum_radius.hpp>
#include <sge/camera/spherical/object.hpp>
#include <sge/camera/spherical/parameters.hpp>
#include <sge/camera/spherical/action/wasd_mapping.hpp>
#include <sge/camera/spherical/coordinate_system/azimuth.hpp>
#include <sge/camera/spherical/coordinate_system/homogenous.hpp>
#include <sge/camera/spherical/coordinate_system/inclination.hpp>
#include <sge/camera/spherical/coordinate_system/look_down_positive_z.hpp>
#include <sge/camera/spherical/coordinate_system/object.hpp>
#include <sge/camera/spherical/coordinate_system/radius.hpp>
#include <sge/config/media_path.hpp>
#include <sge/console/arg_list.hpp>
#include <sge/console/gfx.hpp>
#include <sge/console/object.hpp>
#include <sge/console/output_line_limit.hpp>
#include <sge/console/sprite_object.hpp>
#include <sge/console/sprite_parameters.hpp>
#include <sge/console/callback/from_functor.hpp>
#include <sge/console/callback/name.hpp>
#include <sge/console/callback/short_description.hpp>
#include <sge/font/metrics.hpp>
#include <sge/font/metrics_scoped_ptr.hpp>
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
#include <sge/image/colors.hpp>
#include <sge/image/color/init.hpp>
#include <sge/image/color/rgba8.hpp>
#include <sge/image/color/any/object.hpp>
#include <sge/input/keyboard/action.hpp>
#include <sge/input/keyboard/device.hpp>
#include <sge/input/keyboard/key_code.hpp>
#include <sge/media/extension.hpp>
#include <sge/media/extension_set.hpp>
#include <sge/media/optional_extension_set.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/object.hpp>
#include <sge/parse/json/parse_file_exn.hpp>
#include <sge/parse/json/path.hpp>
#include <sge/parse/json/value.hpp>
#include <sge/parse/json/config/create_command_line_parameters.hpp>
#include <sge/parse/json/config/merge_command_line_parameters.hpp>
#include <sge/renderer/depth_stencil_buffer.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/no_multi_sampling.hpp>
#include <sge/renderer/nonindexed_primitive_type.hpp>
#include <sge/renderer/onscreen_target.hpp>
#include <sge/renderer/parameters.hpp>
#include <sge/renderer/resource_flags.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scalar.hpp>
#include <sge/renderer/scoped_block.hpp>
#include <sge/renderer/size_type.hpp>
#include <sge/renderer/vector3.hpp>
#include <sge/renderer/visual_depth.hpp>
#include <sge/renderer/vsync.hpp>
#include <sge/renderer/projection/far.hpp>
#include <sge/renderer/projection/fov.hpp>
#include <sge/renderer/projection/near.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/renderer/state/depth_func.hpp>
#include <sge/renderer/state/dest_blend_func.hpp>
#include <sge/renderer/state/draw_mode.hpp>
#include <sge/renderer/state/float.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/source_blend_func.hpp>
#include <sge/renderer/state/trampoline.hpp>
#include <sge/renderer/texture/address_mode.hpp>
#include <sge/renderer/texture/address_mode2.hpp>
#include <sge/renderer/texture/address_mode_s.hpp>
#include <sge/renderer/texture/address_mode_t.hpp>
#include <sge/renderer/texture/create_planar_from_path.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/renderer/texture/planar_scoped_ptr.hpp>
#include <sge/renderer/texture/planar_shared_ptr.hpp>
#include <sge/renderer/texture/set_address_mode2.hpp>
#include <sge/renderer/texture/stage.hpp>
#include <sge/renderer/texture/filter/point.hpp>
#include <sge/renderer/texture/filter/scoped.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/sprite/object_impl.hpp>
#include <sge/sprite/parameters_impl.hpp>
#include <sge/systems/cursor_option.hpp>
#include <sge/systems/cursor_option_field.hpp>
#include <sge/systems/font.hpp>
#include <sge/systems/image2d.hpp>
#include <sge/systems/input.hpp>
#include <sge/systems/input_helper.hpp>
#include <sge/systems/input_helper_field.hpp>
#include <sge/systems/instance.hpp>
#include <sge/systems/list.hpp>
#include <sge/systems/renderer.hpp>
#include <sge/systems/window.hpp>
#include <sge/texture/part_raw.hpp>
#include <sge/texture/part_shared_ptr.hpp>
#include <sge/timer/basic.hpp>
#include <sge/timer/elapsed.hpp>
#include <sge/timer/elapsed_and_reset.hpp>
#include <sge/timer/frames_counter.hpp>
#include <sge/timer/parameters.hpp>
#include <sge/timer/reset_when_expired.hpp>
#include <sge/timer/clocks/standard.hpp>
#include <sge/viewport/fill_on_resize.hpp>
#include <sge/viewport/manager.hpp>
#include <sge/window/dim.hpp>
#include <sge/window/parameters.hpp>
#include <sge/window/system.hpp>
#include <sge/window/title.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/extract_from_string_exn.hpp>
#include <fcppt/format.hpp>
#include <fcppt/insert_to_string.hpp>
#include <fcppt/make_shared_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/scoped_ptr.hpp>
#include <fcppt/shared_ptr.hpp>
#include <fcppt/string.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/math/deg_to_rad.hpp>
#include <fcppt/math/pi.hpp>
#include <fcppt/math/twopi.hpp>
#include <fcppt/math/box/structure_cast.hpp>
#include <fcppt/signal/scoped_connection.hpp>
#include <fcppt/thread/object.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/function.hpp>
#include <boost/chrono/duration.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/spirit/home/phoenix/core/reference.hpp>
#include <boost/spirit/home/phoenix/operator/self.hpp>
#include <algorithm>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <fcppt/config/external_end.hpp>


namespace
{

void
toggle_console(
	sge::console::gfx &console,
	sge::camera::has_activation &camera)
{
	bool const act = console.active();
	console.active(!act);
	camera.is_active(act);
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
		throw e;
	}
}

void
quit(
	bool &_value)
{
	_value = false;
}

void
decrement_opacity(
	sgevollib::cube::object &_cube)
{
	_cube.opacity(
			_cube.opacity() * .5f);
}

void
increment_opacity(
	sgevollib::cube::object &_cube)
{
	_cube.opacity(
			_cube.opacity() * 2.f);
}

}

awl::main::exit_code const
sgevol_main(
	awl::main::function_context const &_main_function_context)
try
{
	sge::parse::json::object const &config_file =
		sge::parse::json::config::merge_command_line_parameters(
			sge::parse::json::parse_file_exn(
				sgevollib::media_path()/FCPPT_TEXT("config.json")),
			sge::parse::json::config::create_command_line_parameters(
				_main_function_context.argc(),
				_main_function_context.argv()));

	fcppt::string console_bg =
		sge::parse::json::find_and_convert_member<fcppt::string>(
			config_file,
			sge::parse::json::path(
				FCPPT_TEXT("console"))
			/ FCPPT_TEXT("bg"));
	sge::image::color::rgba8 console_fg =
		sgevollib::json::parse_color<sge::image::color::rgba8>(
			sge::parse::json::find_and_convert_member<sge::parse::json::value>(
				config_file,
			sge::parse::json::path(
				FCPPT_TEXT("console"))
			/ FCPPT_TEXT("fg")));
	sge::console::sprite_object::dim console_dim =
		sge::parse::json::find_and_convert_member<
			sge::console::sprite_object::dim
		>(
			config_file,
			sge::parse::json::path(
				FCPPT_TEXT("console"))
			/ FCPPT_TEXT("dim"));

	sge::window::dim window_dim =
		sge::parse::json::find_and_convert_member<
			sge::window::dim
		>(
			config_file,
			sge::parse::json::path(
				FCPPT_TEXT("window-dim")));
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
	fcppt::string texture_path =
		sge::parse::json::find_and_convert_member<fcppt::string>(
			config_file,
			sge::parse::json::path(
			FCPPT_TEXT("path")));
	sge::renderer::scalar opacity_factor =
		sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
			config_file,
			sge::parse::json::path(
				FCPPT_TEXT("opacity-factor")));
	sge::image::color::rgba8 background_color =
		sgevollib::json::parse_color<sge::image::color::rgba8>(
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
	bool spherical_texture =
		sge::parse::json::find_and_convert_member<bool>(
			config_file,
			sge::parse::json::path(
			FCPPT_TEXT("spherical-texture")));
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
	sge::renderer::scalar cam_acc_factor =
		sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				config_file,
			sge::parse::json::path(
				FCPPT_TEXT("cam")) /
				FCPPT_TEXT("acc-factor"));
	sge::renderer::scalar cam_min_radius =
		sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				config_file,
			sge::parse::json::path(
				FCPPT_TEXT("cam")) /
				FCPPT_TEXT("min-radius"));
	sge::renderer::scalar cam_max_radius =
		sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				config_file,
			sge::parse::json::path(
				FCPPT_TEXT("cam")) /
				FCPPT_TEXT("max-radius"));
	sge::renderer::vector3 cam_damping =
		sge::parse::json::find_and_convert_member<sge::renderer::vector3>(
				config_file,
			sge::parse::json::path(
				FCPPT_TEXT("cam")) /
				FCPPT_TEXT("damping"));
	// nonsensical...
	if (load_texture)
		save_texture = false;

	bool running = false;

	sge::window::dim const dimensions(
		window_dim
	);

	sge::systems::instance const sys(
		sge::systems::list()
		(sge::systems::window(
				sge::window::parameters(
					// Fenstertitel offenbar
					sge::window::title(
						FCPPT_TEXT("sgevollib: cloud volume rendering demo")
					),
					dimensions)))
		(sge::systems::renderer(
				sge::renderer::parameters(
					sge::renderer::visual_depth::depth32,
					sge::renderer::depth_stencil_buffer::d24,
					sge::renderer::vsync::on,
					sge::renderer::no_multi_sampling),
				sge::viewport::fill_on_resize()))
		(sge::systems::image2d(
			sge::image::capabilities_field::null(),
			sge::media::optional_extension_set(
				fcppt::assign::make_container<sge::media::extension_set>(
					sge::media::extension(
						FCPPT_TEXT("png"))))))
		(sge::systems::input(
				sge::systems::input_helper_field(
					sge::systems::input_helper::keyboard_collector) |
					sge::systems::input_helper::mouse_collector,
				sge::systems::cursor_option_field(
					sge::systems::cursor_option::exclusive)))
		(sge::systems::font())
	);

	// Abkürzung, damit wir nicht immer sys.renderer() schreiben müssen
	sge::renderer::device& rend(
		sys.renderer());

	sge::font::metrics_scoped_ptr const metrics(
		sys.font_system().create_font(
				sge::config::media_path()
				/ FCPPT_TEXT("fonts")
				/ FCPPT_TEXT("default.ttf"),
				static_cast<sge::font::size_type>(96)));

	sge::font::text::drawer_3d drawer(
			rend,
			sge::image::colors::white()
	);
	// typedef fcppt::scoped_ptr<texture3d> texture_scoped_ptr;

	sgevollib::texture3d mytex(
		texture_size);

	sgevollib::noise_volume noise(
		noise_size);

	boost::function<void()> tex_action;

	if (load_texture)
		tex_action =
			std::tr1::bind(
				&sgevollib::texture3d::load,
				&mytex,
				boost::filesystem::path(texture_path));
	else
		tex_action =
			spherical_texture
				?
				std::tr1::bind(
					&sgevollib::texture3d::fill_spherical,
					&mytex)
				:
				std::tr1::bind(
					&sgevollib::texture3d::fill,
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

	sge::camera::spherical::object cam(
		sge::camera::spherical::parameters(
			sys.keyboard_collector(),
			sge::camera::spherical::is_active(
				true),
			sge::camera::spherical::coordinate_system::look_down_positive_z(
				sge::camera::spherical::coordinate_system::radius(
					3.0f)),
			sge::camera::spherical::action::wasd_mapping())
			.maximum_radius(
				sge::camera::spherical::maximum_radius(
					cam_max_radius))
			.minimum_radius(
				sge::camera::spherical::minimum_radius(
					cam_min_radius))
			.damping_factor(
				sge::camera::spherical::damping_factor(
					sge::camera::spherical::coordinate_system::object(
						sge::camera::spherical::coordinate_system::azimuth(
							cam_damping.x()),
						sge::camera::spherical::coordinate_system::inclination(
							cam_damping.y()),
						sge::camera::spherical::coordinate_system::radius(
							cam_damping.z()))))
			.acceleration_factor(
				sge::camera::spherical::acceleration_factor(
					sge::camera::spherical::coordinate_system::homogenous(
						cam_acc_factor))));

	sge::camera::perspective_projection_from_viewport viewport_connection(
		cam,
		rend,
		sys.viewport_manager(),
		sge::renderer::projection::near(
			0.1f),
		sge::renderer::projection::far(
			1000.f),
		sge::renderer::projection::fov(
			fcppt::math::deg_to_rad(
				90.f)));
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
			boost::chrono::milliseconds(
				100)));
	std::cout << "timer initialized" << std::endl;

	float progress = 0.f;
	while( !aborted )
	{
		if(sge::timer::reset_when_expired(accesstimer))
			if((progress = mytex.progress()) >= 99.f)
				break;

		sys.window_system().poll();

		sge::renderer::scoped_block const block_(rend);

		sge::font::text::draw(
			*metrics,
			drawer,
			fcppt::insert_to_string<sge::font::text::string>(
				static_cast<int>(progress)
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
		return awl::main::exit_failure();

	load_thread.join();

	if (save_texture)
		mytex.save(
			boost::filesystem::path(
			texture_path));

	sge::renderer::texture::planar_shared_ptr phase_tex(
		sge::renderer::texture::create_planar_from_path(
			sgevollib::media_path()
				/ FCPPT_TEXT("textures")
				/ FCPPT_TEXT("phase.png"),
				rend,
				sys.image_system(),
				sge::renderer::texture::mipmap::off(),
					sge::renderer::resource_flags::none));

	sgevollib::cube::object cube(
		rend,
		sgevollib::media_path()
			/ FCPPT_TEXT("shaders")
			/ FCPPT_TEXT("vertex")
			/ FCPPT_TEXT("vertex.glsl"),
		sgevollib::media_path()
			/ FCPPT_TEXT("shaders")
			/ FCPPT_TEXT("fragment")
			/ (fcppt::format(FCPPT_TEXT("%1%.glsl")) % shader_file).str(),
		cam,
		opacity_factor,
		mytex.const_view(),
		noise.const_view(),
		phase_tex);

	sgevollib::stars::object stars(
		star_count,
		star_size,
		rend,
		sgevollib::media_path()
			/ FCPPT_TEXT("shaders")
			/ FCPPT_TEXT("vertex")
			/ FCPPT_TEXT("stars.glsl"),
		sgevollib::media_path()
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
			boost::chrono::seconds(
				1)));

	sge::timer::frames_counter fps_counter;

	sge::font::metrics_scoped_ptr const fps_metrics(
		sys.font_system().create_font(
				sge::config::media_path()
				/ FCPPT_TEXT("fonts")
				/ FCPPT_TEXT("default.ttf"),
				static_cast<sge::font::size_type>(32)
		)
	);

	sge::timer::basic<sge::timer::clocks::standard> offset_timer(
		sge::timer::parameters<sge::timer::clocks::standard>(
			boost::chrono::milliseconds(
				40)));

	float offset = 0.f;

	// console begin
	sge::console::object console(
		SGE_FONT_TEXT_LIT('/')
	);

	sge::renderer::texture::planar_scoped_ptr const console_tex(
		sge::renderer::texture::create_planar_from_path(
			sgevollib::media_path()
				/ FCPPT_TEXT("textures")
				/ console_bg,
				rend,
				sys.image_system(),
				sge::renderer::texture::mipmap::off(),
					sge::renderer::resource_flags::none
				));

	fcppt::signal::scoped_connection const quit_conn(
		console.insert(
			sge::console::callback::from_functor<void()>(
				std::tr1::bind(
					&::quit,
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
					static_cast<void(sgevollib::cube::object::*)(sge::renderer::scalar)>(&sgevollib::cube::object::opacity),
					&cube,
					std::tr1::placeholders::_1),
				sge::console::callback::name(
					SGE_FONT_TEXT_LIT("opacity")),
				sge::console::callback::short_description(
					SGE_FONT_TEXT_LIT("cloud opacity")))));

	sge::font::metrics_scoped_ptr const console_metrics(
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
		sge::image::color::any::object(console_fg),
		*console_metrics,
		sys.keyboard_collector(),
		sge::console::sprite_object(
			sge::console::sprite_parameters()
			.pos(
				sge::console::sprite_object::vector::null())
			.texture(
				fcppt::make_shared_ptr<sge::texture::part_raw>(fcppt::ref(*console_tex)))
			.size(
				console_dim)),
		static_cast<
			sge::console::output_line_limit
		>(
			100));

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

	fcppt::signal::scoped_connection decrement_opacity_conn(
		sys.keyboard_collector().key_callback(
			sge::input::keyboard::action(
				sge::input::keyboard::key_code::lbracket,
				std::tr1::bind(
					&::decrement_opacity,
					fcppt::ref(cube)))));

	fcppt::signal::scoped_connection increment_opacity_conn(
		sys.keyboard_collector().key_callback(
			sge::input::keyboard::action(
				sge::input::keyboard::key_code::rbracket,
				std::tr1::bind(
					&::increment_opacity,
					fcppt::ref(cube)))));

	// console end
	while(running)
	{
		// Sonst werden keine Input-Events geschickt
		sys.window_system().poll();

		cam.update(
			sge::timer::elapsed_and_reset<sge::camera::update_duration>(
				frame_timer));

		if (sge::timer::reset_when_expired(offset_timer))
			offset += fcppt::math::pi<float>()/80.f;
		if (offset > fcppt::math::twopi<float>())
			offset = 0.f;

		// Beginne Renderdurchgang
		sge::renderer::scoped_block const block_(rend);

		//stars.render();

		cube.render(offset);

		sge::font::text::draw(
			*fps_metrics,
			drawer,
			sge::font::text::from_fcppt_string(
				fps_counter.frames_str()),
			fcppt::math::box::structure_cast<sge::font::rect>(
				rend.onscreen_target().viewport().get()),
		sge::font::text::align_h::left,
			sge::font::text::align_v::top,
			sge::font::text::flags::none
		);

		fps_counter.update();

		if (console_gfx.active())
		{
			sge::renderer::texture::set_address_mode2(
				rend,
				sge::renderer::texture::stage(0u),
				sge::renderer::texture::address_mode2(
					sge::renderer::texture::address_mode_s(
						sge::renderer::texture::address_mode::clamp),
					sge::renderer::texture::address_mode_t(
						sge::renderer::texture::address_mode::repeat)));
			console_gfx.render();
		}
	}

	return
		sys.window_system().exit_code();
}
catch(sge::exception const &e)
{
	fcppt::io::cerr() << e.string() << FCPPT_TEXT('\n');
	return awl::main::exit_failure();
}
catch(fcppt::exception const &e)
{
	fcppt::io::cerr() << e.string() << FCPPT_TEXT("\n");
	return awl::main::exit_failure();
}
catch(std::exception const &e)
{
	std::cerr << e.what() << FCPPT_TEXT('\n');
	return awl::main::exit_failure();
}
