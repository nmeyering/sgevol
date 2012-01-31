#include <fcppt/math/vector/vector.hpp>
#include <sgevollib/simplex_noise.hpp>
#include <sge/image2d/l8.hpp>
#include <sge/image/store.hpp>
#include <sge/image2d/dim.hpp>
#include <sge/image2d/save_from_view.hpp>
#include <sge/image2d/view/view.hpp>
#include <sge/systems/instance.hpp>
#include <sge/systems/list.hpp>
#include <sge/systems/image2d.hpp>
#include <mizuiro/color/channel/luminance.hpp>
#include <sge/image/capabilities_field.hpp>
#include <sge/media/extension.hpp>
#include <sge/media/extension_set.hpp>
#include <fcppt/assign/make_container.hpp>
#include <sge/media/optional_extension_set.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

int
main(
	const int argc,
	char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "usage: " << argv[0] << " <file>" << std::endl;
		return EXIT_FAILURE;
	}

	sge::systems::instance const sys(
			sge::systems::list()
			(sge::systems::image2d(
				sge::image::capabilities_field::null(),
				sge::media::optional_extension_set(
					fcppt::assign::make_container<sge::media::extension_set>(
						sge::media::extension(
							FCPPT_TEXT("png")))))));

	std::ifstream file(argv[1]);

	if (!file.is_open())
		return EXIT_FAILURE;

	unsigned w, h;
	if(!(file >> w >> h))
		return EXIT_FAILURE;

	// consume rest of first line...
	std::string foo;
	std::getline(file, foo);

	std::cout << "width: " << w << ", height: " << h << std::endl;

	// std::vector<float> vec;
	// vec.reserve(w * h);

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

	sge::image2d::save_from_view(
		sys.image_system(),
		sge::image2d::view::to_const(
			sge::image2d::view::object(
				store.wrapped_view())),
		fcppt::filesystem::path(
			argv[2]));

	file.close();
		return EXIT_SUCCESS;
}
