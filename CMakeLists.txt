CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(sgevol)

# This allows ENDIFs with no specifier _what_ ends
SET (CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)

# So we can use FindFcppt.cmake, among others
SET (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

IF(${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
	MESSAGE(
		FATAL_ERROR
		"In-source builds are not permitted. "
		"Make a separate folder for building:\n"
		"mkdir build; cd build; cmake ..\n"
		"Before that, remove the files that cmake"
		"just created:\nrm -rf CMakeCache.txt CMakeFiles")
ENDIF()

# boost begin
SET(Boost_ADDITIONAL_VERSIONS "1.41" "1.41.0" "1.42" "1.42.0" "1.43" "1.43.0" "1.44" "1.44.0" "1.45.0")

FIND_PACKAGE(
	Boost 1.45.0 REQUIRED COMPONENTS
	system
  program_options)

INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIR})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
# boost end

# fcppt begin
FIND_PACKAGE(
	Fcppt REQUIRED)

INCLUDE_DIRECTORIES(${FCPPT_INCLUDE_DIRS})
LINK_DIRECTORIES(${FCPPT_LIBRARY_DIRS})
# fcppt end

# sge begin
FIND_PACKAGE(
	SGE REQUIRED)

INCLUDE_DIRECTORIES(${SGE_INCLUDE_DIRS})

ADD_DEFINITIONS(${SGE_CFLAGS})
# sge end

# clang check begin
INCLUDE (CheckCXXSourceRuns)

CHECK_CXX_SOURCE_RUNS(
	"#include <cstdlib>
	int main()
	{
	#ifdef __clang__
		return EXIT_SUCCESS;
	#else
		return EXIT_FAILURE;
	#endif
	}
	"
	COMPILER_IS_CLANGPP)
# clang check end

IF (CMAKE_COMPILER_IS_GNUCXX OR COMPILER_IS_CLANGPP)
	ADD_DEFINITIONS("
		-fvisibility=hidden -ansi -pedantic-errors -Wall"
		"-Wextra -Wconversion -Wfloat-equal -Wredundant-decls"
		"-Winit-self -Woverloaded-virtual"
		"-Wshadow"
		"-Wold-style-cast -Wnon-virtual-dtor -Wctor-dtor-privacy -Wsign-promo"
		"-Wstrict-aliasing=1 -Wmissing-declarations -Wno-long-long")
	# we don't use long long but boost does :(

	ADD_DEFINITIONS(-D BOOST_FILESYSTEM_NO_DEPRECATED)
ENDIF ()

# TODO
SET(
	media_path
	${CMAKE_INSTALL_PREFIX}/share/games/${PROJECT_NAME})

# TODO
SET_SOURCE_FILES_PROPERTIES(
	media_path.cpp
	PROPERTIES
	COMPILE_FLAGS
	-DMEDIA_PATH="\\\"${media_path}\\\""
)

SET_SOURCE_FILES_PROPERTIES(
	media_path.cpp
	PROPERTIES
	COMPILE_FLAGS
	-DMEDIA_PATH="\\\"${CMAKE_INSTALL_PREFIX}/share/games/${PROJECT_NAME}/media\\\""
)

ADD_EXECUTABLE(
	app
  media_path.cpp
  texture3d.cpp
	perlin3d.cpp
)

ADD_EXECUTABLE(
	test
	perlin3d.cpp
	test.cpp
)

TARGET_LINK_LIBRARIES(
	app
	${FCPPT_LIBRARIES}
	${SGE_CORE_LIBRARY}
	${SGE_SHADER_LIBRARY}
	${SGE_CONSOLE_LIBRARY}
	${SGE_SYSTEMS_LIBRARY}
	${SGE_CAMERA_LIBRARY}
	${SGE_PARSE_LIBRARY}
	${SGE_SPRITE_LIBRARY}
	${Boost_LIBRARIES})

TARGET_LINK_LIBRARIES(
	test
	${FCPPT_LIBRARIES}
	${SGE_CORE_LIBRARY}
	${SGE_SHADER_LIBRARY}
	${SGE_CONSOLE_LIBRARY}
	${SGE_SYSTEMS_LIBRARY}
	${SGE_CAMERA_LIBRARY}
	${SGE_PARSE_LIBRARY}
	${SGE_SPRITE_LIBRARY}
	${Boost_LIBRARIES})