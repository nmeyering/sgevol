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
#include <fcppt/math/clamp.hpp>
#include <fcppt/math/twopi.hpp>
#include <sge/media/optional_extension_set.hpp>
#include <boost/lexical_cast.hpp>
#include <cmath>
#include <iostream>
#include <ostream>

namespace
{

// theta is in [0,1] (cosine of an angle)
// g is in [0,1] as well
double
phase(
	double theta,
	double g)
{
	return
		std::pow(
			(g - 1.0) * (g - 1.0) /
				(1.0 + g * g -
				 2.0 * g *
				 std::cos(fcppt::math::twopi<double>() * theta)),
			1.5);
}

}

int
main(
	const int argc,
	char* argv[])
{
	if (argc != 4)
	{
		std::cerr << "usage: " << argv[0] << "<w> <h> <file>" << std::endl;
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


	unsigned w = boost::lexical_cast<unsigned>(argv[1]);
	unsigned h = boost::lexical_cast<unsigned>(argv[2]);

	sge::image2d::l8 store(sge::image2d::dim(w,h));

	for(unsigned j = 0; j < h; ++j)
	{
		for(unsigned i = 0; i < w; ++i)
		{
			store.view()[
					sge::image2d::l8::view_type::dim(
							i,
							j)
				].set(
					mizuiro::color::channel::luminance(),
					static_cast<boost::uint8_t>(
						256.0 *
						phase(
							static_cast<double>(i) / static_cast<double>(w),
							static_cast<double>(j) / static_cast<double>(h))));
		}
	}

	sge::image2d::save_from_view(
		sys.image_system(),
		sge::image2d::view::to_const(
			sge::image2d::view::object(
				store.wrapped_view())),
		fcppt::filesystem::path(
			argv[3]));

	return EXIT_SUCCESS;
}