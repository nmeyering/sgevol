#include "media_path.hpp"
#include <sgevol/build/media_path.hpp>

// This is taken literally from sanguis' media_path.cpp

fcppt::filesystem::path const
sgevol::media_path()
{
	return sgevol::build_media_path();
}

