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
#include <iostream>
#include <cstdlib>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr
			<< "Usage: "
			<< argv[0]
			<< " OUTPUT_FILE"
			<< std::endl;
		return EXIT_FAILURE;
	}

	typedef fcppt::math::vector::static_<double,2>::type vec2;

	sgevollib::simplex_noise<double,2> noise(128, 128);

	sge::systems::instance const sys(
			sge::systems::list()
			(sge::systems::image2d(
				sge::image::capabilities_field::null(),
				sge::media::optional_extension_set(
					fcppt::assign::make_container<sge::media::extension_set>(
						sge::media::extension(
							FCPPT_TEXT("png")))))));

	unsigned const w = 256, h = 256;
	double const scale = 10.0;

	sge::image2d::l8 store(sge::image2d::dim(w,h));
	for (unsigned y = 0; y < h; ++y)
		for (unsigned x = 0; x < w; ++x)
			store.view()[
					sge::image2d::l8::view_type::dim(
							x,
							y)
				].set(
					mizuiro::color::channel::luminance(),
					static_cast<boost::uint8_t>(noise.sample(scale * vec2(
								static_cast<double>(x),
								static_cast<double>(y)
								))));

	sge::image2d::save_from_view(
		sys.image_system(),
		sge::image2d::view::to_const(
			sge::image2d::view::object(
				store.wrapped_view())),
		fcppt::filesystem::path(
			argv[1]));

	return EXIT_SUCCESS;
}
