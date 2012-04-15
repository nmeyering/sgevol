#include <sgevollib/simplex_noise.hpp>
#include <awl/main/exit_code.hpp>
#include <awl/main/exit_failure.hpp>
#include <awl/main/exit_success.hpp>
#include <awl/main/function_context.hpp>
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
#include <mizuiro/color/channel/luminance.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/math/vector/static.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <fcppt/config/external_end.hpp>


namespace
{
double
noise(
	sgevollib::simplex_noise<double,2> &_gen,
	unsigned _x,
	unsigned _y,
	double _amplitude,
	double _freq,
	unsigned _octaves)
{
	typedef fcppt::math::vector::static_<double,2>::type vec2;

	double result = 0.0;
	for (unsigned i = 0; i < _octaves; ++i)
		result +=
			std::pow(0.5f, i) * _amplitude *
			_gen.sample(
				std::pow(2, i) * _freq * vec2(
					static_cast<double>(_x),
					static_cast<double>(_y)));
	return 128.0 + 128.0 * result;
}
}

awl::main::exit_code const
sgevol_main(
	awl::main::function_context const &_main_function_context)
{
	if (_main_function_context.argc() != 3)
	{
		std::cerr
			<< "Usage: "
			<< _main_function_context.argv()[0]
			<< " OCTAVES OUTPUT_FILE"
			<< std::endl;
		return awl::main::exit_failure();
	}

	unsigned octaves = boost::lexical_cast<unsigned>(_main_function_context.argv()[1]);

	typedef fcppt::math::vector::static_<double,2>::type vec2;

	sgevollib::simplex_noise<double,2> noise_generator(128, 256);

	sge::systems::instance const sys(
			sge::systems::list()
			(sge::systems::image2d(
				sge::image::capabilities_field::null(),
				sge::media::optional_extension_set(
					fcppt::assign::make_container<sge::media::extension_set>( sge::media::extension(
							FCPPT_TEXT("png")))))));

	unsigned const w = 256, h = 256;
	double const freq = .02, amplitude = 0.55;

	sge::image2d::l8 store(sge::image2d::dim(w,h));
	for (unsigned y = 0; y < h; ++y)
		for (unsigned x = 0; x < w; ++x)
			store.view()[
					sge::image2d::l8::view_type::dim(
							x,
							y)
				].set(
					mizuiro::color::channel::luminance(),
					static_cast<boost::uint8_t>(
								noise(
									noise_generator,
									x,
									y,
									amplitude,
									freq,
									octaves)
					));

	sge::image2d::save_from_view(
		sys.image_system(),
		sge::image2d::view::to_const(
			sge::image2d::view::object(
				store.wrapped_view())),
		boost::filesystem::path(
			_main_function_context.argv()[2]));

	return awl::main::exit_success();
}
