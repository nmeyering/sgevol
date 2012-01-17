#include <sgevol/build/media_path.hpp>
#include <sgevollib/media_path.hpp>
#include <fcppt/filesystem/path.hpp>


fcppt::filesystem::path const
sgevollib::media_path()
{
	return sgevol::build_media_path();
}
