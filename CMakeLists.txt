cmake_minimum_required(VERSION 2.6)

project(sgevol)

include(FcpptCMakeUtils)
# TODO
# switch to new AWL main

# boost begin
find_package(
	Boost 1.47.0 REQUIRED COMPONENTS
	chrono
)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
# boost end

# fcppt begin
find_package(
	Fcppt REQUIRED)
include_directories(${Fcppt_INCLUDE_DIRS})
add_definitions(${Fcppt_DEFINITIONS})
# fcppt end

# awl begin
find_package(Awl REQUIRED)
include_directories(${Awl_INCLUDE_DIRS})
add_definitions(${Awl_DEFINITIONS})
#awl end

# mizuiro begin
find_package(Mizuiro REQUIRED)
include_directories(${Mizuiro_INCLUDE_DIRS})
add_definitions(${Mizuiro_DEFINITIONS})
#mizuiro end


set(
	INSTALL_MEDIA_DIR
	media
)

include(
	SGECustomPath
)

sge_config_add_custom_path(
	sgevol
	media
	${CMAKE_SOURCE_DIR}/media
	${INSTALL_MEDIA_DIR}
)

# sge begin
find_package(
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

include_directories(${SGE_INCLUDE_DIRS})
add_definitions(${SGE_CFLAGS})
# sge end

include_directories(${CMAKE_SOURCE_DIR}/src)

set(
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
fcppt_utils_add_source_groups(
	"${SGEVOLLIB_FILES}"
)

add_library(
	sgevollib STATIC ${SGEVOLLIB_FILES})

add_executable(
	sgevol
	src/sgevol/main.cpp
)

target_link_libraries(
	sgevol
	sgevollib
	${Boost_LIBRARIES}
	${Fcppt_core_LIBRARIES}
	${Fcppt_filesystem_LIBRARIES}
	${Fcppt_thread_LIBRARIES}
	${SGE_LIBRARIES}
)


add_executable(
	cubetest
	src/sgevol/cubetest.cpp
)

target_link_libraries(
	cubetest
	sgevollib
	${Boost_LIBRARIES}
	${Fcppt_core_LIBRARIES}
	${Fcppt_filesystem_LIBRARIES}
	${Fcppt_thread_LIBRARIES}
	${SGE_LIBRARIES}
)

add_executable(
	planar_noise
	src/sgevol/planar_noise.cpp
)

target_link_libraries(
	planar_noise
	sgevollib
	${Boost_LIBRARIES}
	${Fcppt_core_LIBRARIES}
	${Fcppt_filesystem_LIBRARIES}
	${Fcppt_thread_LIBRARIES}
	${SGE_LIBRARIES}
)

add_executable(
	cloud_data_to_png
	src/sgevol/cloud_data_to_png.cpp
)

target_link_libraries(
	cloud_data_to_png
	sgevollib
	${Boost_LIBRARIES}
	${Fcppt_core_LIBRARIES}
	${Fcppt_filesystem_LIBRARIES}
	${Fcppt_thread_LIBRARIES}
	${SGE_LIBRARIES}
)

add_executable(
	create_phase_function
	src/sgevol/create_phase_function.cpp
)

target_link_libraries(
	create_phase_function
	sgevollib
	${Boost_LIBRARIES}
	${Fcppt_core_LIBRARIES}
	${Fcppt_filesystem_LIBRARIES}
	${Fcppt_thread_LIBRARIES}
	${SGE_LIBRARIES}
)
