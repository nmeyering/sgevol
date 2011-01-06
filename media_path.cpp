#include "media_path.hpp"
#include <fcppt/filesystem/path.hpp>
#include <sge/config/find_path_exn.hpp>
#include <sge/config/make_files.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/text.hpp>

// This is taken literally from sanguis' media_path.cpp
#define SGEVOL_FCPPT_TEXT_WRAPPER(\
		text\
)\
	FCPPT_TEXT(\
		text\
	)

fcppt::filesystem::path const
sgevol::media_path()
{
	static fcppt::filesystem::path const ret(
		sge::config::find_path_exn(
			sge::config::make_files(
				FCPPT_TEXT("sgevol")
			),
			FCPPT_TEXT("sgevol_media"),
			fcppt::assign::make_container<
				sge::config::path_vector
			>
			(
				FCPPT_TEXT("media")
			)
			(
				SGEVOL_FCPPT_TEXT_WRAPPER(
					MEDIA_PATH
				)
			)
		)
	);

	return ret; 
}

