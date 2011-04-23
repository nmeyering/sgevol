#include "test.hpp"

int main(int argc, char **argv)
{
	std::size_t dim;
	std::size_t n;
	std::size_t octaves;
	if (argc == 4)
	{
		dim = boost::lexical_cast<std::size_t>(argv[1]);
		n = boost::lexical_cast<std::size_t>(argv[2]);
		octaves = boost::lexical_cast<std::size_t>(argv[3]);
	}
	else
	{
		dim = 512;
		n = 3;
		octaves = 2;
	}

	switch(n)
	{
		case 2:
			test<2>(dim,octaves);
			break;
		case 3:
			test<3>(dim,octaves);
			break;
		case 4:
			test<4>(dim,octaves);
			break;
	}
	/*
	output_grid output(
		output_grid::dim(
			dim,
			dim));
	
	for (output_grid::size_type y = 0; y < dim; ++y)
		for (output_grid::size_type x = 0; x < dim; ++x)
		{
			output[
				output_grid::dim(
					x,
					y)
			] = static_cast<unsigned char>(
			256 *
			(0.5f + 0.5f *
			tmpgrid[
				grid::dim(
					x,
					y
				)
			]));
		}

	to_ppm(
		output,
		std::cout,
		static_cast<unsigned char>(255));
	*/

	//std::cout << noisy.sample(vec(0.5f,0.78f)) << std::endl;
}
