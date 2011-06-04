cmake_minimum_required(VERSION 2.6)

project(sgevol)

include(FcpptCMakeUtils)

# boost begin
find_package(
	Boost 1.45.0 REQUIRED COMPONENTS
	system
  )
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
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
#awl end

# mizuiro begin
find_package(Mizuiro REQUIRED)
include_directories(${Mizuiro_INCLUDE_DIRS})
#mizuiro end

# sge begin
find_package(
	SGE REQUIRED COMPONENTS
	shader config time systems camera fonttext)
include_directories(${SGE_INCLUDE_DIRS})
add_definitions(${SGE_CFLAGS})
# sge end

# TODO
set(
	media_path
	${CMAKE_INSTALL_PREFIX}/share/games/${PROJECT_NAME})

# TODO
set_source_files_properties(
	media_path.cpp
	PROPERTIES
	COMPILE_FLAGS
	-DMEDIA_PATH="\\\"${media_path}\\\""
)

set_source_files_properties(
	media_path.cpp
	PROPERTIES
	COMPILE_FLAGS
	-DMEDIA_PATH="\\\"${CMAKE_INSTALL_PREFIX}/share/games/${PROJECT_NAME}/media\\\""
)

add_executable(
	test
	test.cpp
)

target_link_libraries(
	test
	${Fcppt_LIBRARIES}
)

add_executable(
	app
	create_cube.cpp
	create_shader.cpp
	main.cpp
  media_path.cpp
  texture3d.cpp
)

target_link_libraries(
	app
	${Boost_LIBRARIES}
	${Fcppt_LIBRARIES}
	${SGE_LIBRARIES}
)
