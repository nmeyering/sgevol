#include <sgevollib/simplex_noise.hpp>
#include <sge/image/capabilities_field.hpp>
#include <sge/image/store.hpp>
#include <sge/image2d/dim.hpp>
#include <sge/image2d/l8.hpp>
#include <sge/image2d/save_from_view.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/image2d/view/object.hpp>
#include <sge/image2d/view/to_const.hpp>
#include <sge/media/extension.hpp>
#include <sge/media/extension_set.hpp>
#include <sge/media/optional_extension_set.hpp>
#include <sge/systems/image2d.hpp>
#include <sge/systems/instance.hpp>
#include <sge/systems/list.hpp>
#include <awl/main/exit_code.hpp>
#include <awl/main/exit_failure.hpp>
#include <awl/main/exit_success.hpp>
#include <awl/main/function_context.hpp>
#include <mizuiro/color/channel/luminance.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <fcppt/config/external_end.hpp>


awl::main::exit_code const
sgevol_main(
	awl::main::function_context const &_main_function_context)
{
	if (_main_function_context.argc() < 2)
	{
		std::cerr << "usage: " << _main_function_context.argv()[0] << " <file>" << std::endl;
		return awl::main::exit_failure();
	}

	sge::systems::instance const sys(
			sge::systems::list()
			(sge::systems::image2d(
				sge::image::capabilities_field::null(),
				sge::media::optional_extension_set(
					fcppt::assign::make_container<sge::media::extension_set>(
						sge::media::extension(
							FCPPT_TEXT("png")))))));

	std::ifstream file(_main_function_context.argv()[1]);

	if (!file.is_open())
		return awl::main::exit_failure();

	unsigned w, h;
	if(!(file >> w >> h))
		return awl::main::exit_failure();

	// consume rest of first line...
	std::string foo;
	std::getline(file, foo);

	std::cout << "width: " << w << ", height: " << h << std::endl;

	for(unsigned frame = 0; frame < 100; ++frame)
	{
		sge::image2d::l8 store(sge::image2d::dim(w,h));

		float f = 0.f;
		float const size = 256.f;

		for(unsigned line = 0; line < w * h && (file >> f); ++line)
		{
			store.view()[
					sge::image2d::l8::view_type::dim(
							line % w,
							line / w)
				].set(
					mizuiro::color::channel::luminance(),
					static_cast<boost::uint8_t>(
						f * size));
		}

		std::string filename(
				_main_function_context.argv()[2]);
		filename += "_";
		filename += boost::lexical_cast<std::string>(frame);
		filename += ".png";

		sge::image2d::save_from_view(
			sys.image_system(),
			sge::image2d::view::to_const(
				sge::image2d::view::object(
					store.wrapped_view())),
			boost::filesystem::path(
				filename));
	}

	file.close();
	return awl::main::exit_success();
}
