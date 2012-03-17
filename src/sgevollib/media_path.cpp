#include <sgevol/build/media_path.hpp>
#include <sgevollib/media_path.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/path.hpp>
#include <fcppt/config/external_end.hpp>


boost::filesystem::path const
sgevollib::media_path()
{
	return sgevol::build_media_path();
}
