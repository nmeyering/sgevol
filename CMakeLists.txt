CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(sgevol)

INCLUDE(FcpptCMakeUtils)

# boost begin
FIND_PACKAGE(
	Boost 1.45.0 REQUIRED COMPONENTS
	system
)

INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
# boost end

# fcppt begin
FIND_PACKAGE(
	Fcppt REQUIRED)
INCLUDE_DIRECTORIES(${Fcppt_INCLUDE_DIRS})
ADD_DEFINITIONS(${Fcppt_DEFINITIONS})
# fcppt end

# awl begin
FIND_PACKAGE(Awl REQUIRED)
INCLUDE_DIRECTORIES(${Awl_INCLUDE_DIRS})
ADD_DEFINITIONS(${Awl_DEFINITIONS})
#awl end

# mizuiro begin
FIND_PACKAGE(Mizuiro REQUIRED)
INCLUDE_DIRECTORIES(${Mizuiro_INCLUDE_DIRS})
ADD_DEFINITIONS(${Mizuiro_DEFINITIONS})
#mizuiro end


SET(
	INSTALL_MEDIA_DIR
	media
)

INCLUDE(
	SGECustomPath
)

SGE_CONFIG_ADD_CUSTOM_PATH(
	sgevol
	media
	${CMAKE_SOURCE_DIR}/media
	${INSTALL_MEDIA_DIR}
)

# sge begin
FIND_PACKAGE(
	SGE REQUIRED COMPONENTS
	camera
	config
	console
	core
	fonttext
	image
	image2d
	image3d
	input
	log
	modelobj
	parse
	renderer
	shader
	systems
	texture
	viewport
	window
)

INCLUDE_DIRECTORIES(${SGE_INCLUDE_DIRS})
ADD_DEFINITIONS(${SGE_CFLAGS})
# sge end

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)

SET(
	SGEVOLLIB_FILES
	src/sgevollib/cloud_sphere/model_format.hpp
	src/sgevollib/cloud_sphere/object.cpp
	src/sgevollib/cloud_sphere/object.hpp
	src/sgevollib/cloud_sphere/vf.hpp
	src/sgevollib/cube/object.cpp
	src/sgevollib/cube/object.hpp
	src/sgevollib/cube/vf.hpp
	src/sgevollib/json/parse_color.hpp
	src/sgevollib/json/parse_rgba8_color.cpp
	src/sgevollib/json/parse_rgba8_color.hpp
	src/sgevollib/locked_value.hpp
	src/sgevollib/media_path.cpp
	src/sgevollib/media_path.hpp
	src/sgevollib/model/object.cpp
	src/sgevollib/model/object.hpp
	src/sgevollib/model/vertex_format.hpp
	src/sgevollib/model/vf.hpp
	src/sgevollib/noise_volume.cpp
	src/sgevollib/noise_volume.hpp
	src/sgevollib/perlin2d.cpp
	src/sgevollib/perlin2d.hpp
	src/sgevollib/perlin3d.cpp
	src/sgevollib/perlin3d.hpp
	src/sgevollib/shadow_volume.cpp
	src/sgevollib/shadow_volume.hpp
	src/sgevollib/simplex_noise.hpp
	src/sgevollib/stars/object.cpp
	src/sgevollib/stars/object.hpp
	src/sgevollib/stars/vf.hpp
	src/sgevollib/texture3d.cpp
	src/sgevollib/texture3d.hpp
	src/sgevollib/to_ppm.hpp
	src/sgevollib/vf.hpp
	src/sgevollib/zip_with.hpp
)

# Source groups are a MSVC file
FCPPT_UTILS_ADD_SOURCE_GROUPS(
	"${SGEVOLLIB_FILES}"
)

ADD_LIBRARY(
	sgevollib STATIC ${SGEVOLLIB_FILES})

# TODO
#SET_SOURCE_FILES_PROPERTIES(
#	media_path.cpp
#	PROPERTIES
#	COMPILE_FLAGS
#	-DMEDIA_PATH="\\\"${media_path}\\\""
#)
#
#SET_SOURCE_FILES_PROPERTIES(
#	src/sgevol/media_path.cpp
#	PROPERTIES
#	COMPILE_FLAGS
#	-DMEDIA_PATH="\\\"${CMAKE_INSTALL_PREFIX}/share/games/${PROJECT_NAME}/media\\\""
#)

ADD_EXECUTABLE(
	sgevol
	src/sgevol/main.cpp
)

TARGET_LINK_LIBRARIES(
	sgevol
	sgevollib
	${Boost_LIBRARIES}
	${Fcppt_core_LIBRARIES}
	${Fcppt_filesystem_LIBRARIES}
	${Fcppt_thread_LIBRARIES}
	${SGE_LIBRARIES}
)


ADD_EXECUTABLE(
	cubetest
	src/sgevol/cubetest.cpp
)

TARGET_LINK_LIBRARIES(
	cubetest
	sgevollib
	${Boost_LIBRARIES}
	${Fcppt_core_LIBRARIES}
	${Fcppt_filesystem_LIBRARIES}
	${Fcppt_thread_LIBRARIES}
	${SGE_LIBRARIES}
)

ADD_EXECUTABLE(
	planar_noise
	src/sgevol/planar_noise.cpp
)

TARGET_LINK_LIBRARIES(
	planar_noise
	sgevollib
	${Boost_LIBRARIES}
	${Fcppt_core_LIBRARIES}
	${Fcppt_filesystem_LIBRARIES}
	${Fcppt_thread_LIBRARIES}
	${SGE_LIBRARIES}
)

ADD_EXECUTABLE(
	cloud_data_to_png
	src/sgevol/cloud_data_to_png.cpp
)

TARGET_LINK_LIBRARIES(
	cloud_data_to_png
	sgevollib
	${Boost_LIBRARIES}
	${Fcppt_core_LIBRARIES}
	${Fcppt_filesystem_LIBRARIES}
	${Fcppt_thread_LIBRARIES}
	${SGE_LIBRARIES}
)

ADD_EXECUTABLE(
	create_phase_function
	src/sgevol/create_phase_function.cpp
)

TARGET_LINK_LIBRARIES(
	create_phase_function
	sgevollib
	${Boost_LIBRARIES}
	${Fcppt_core_LIBRARIES}
	${Fcppt_filesystem_LIBRARIES}
	${Fcppt_thread_LIBRARIES}
	${SGE_LIBRARIES}
)
