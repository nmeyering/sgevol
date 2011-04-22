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

INCLUDE_DIRECTORIES(${FCPPT_INCLUDE_DIRS})
LINK_DIRECTORIES(${FCPPT_LIBRARY_DIRS})
# fcppt end

# awl begin
FIND_PACKAGE(AWL REQUIRED)

INCLUDE_DIRECTORIES(${AWL_INCLUDE_DIRS})
#awl end

# mizuiro begin
FIND_PACKAGE(Mizuiro REQUIRED)

INCLUDE_DIRECTORIES(${MIZUIRO_INCLUDE_DIRS})
#mizuiro end

# sge begin
FIND_PACKAGE(
	SGE REQUIRED COMPONENTS shader config time systems camera fonttext)

INCLUDE_DIRECTORIES(${SGE_INCLUDE_DIRS})

ADD_DEFINITIONS(${SGE_CFLAGS})
# sge end

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
	test
	test.cpp
	simplex2d.cpp
  simplex3d.cpp
)

TARGET_LINK_LIBRARIES(
	test
	${FCPPT_LIBRARIES}
)

ADD_EXECUTABLE(
	app
	create_cube.cpp
	main.cpp
	perlin3d.cpp
  media_path.cpp
  texture3d.cpp
)

TARGET_LINK_LIBRARIES(
	app
	${FCPPT_LIBRARIES}
	${SGE_CORE_LIBRARY}
	${SGE_shader_LIBRARY}
	${SGE_config_LIBRARY}
	${SGE_time_LIBRARY}
	${SGE_systems_LIBRARY}
	${SGE_camera_LIBRARY}
	${SGE_fonttext_LIBRARY}
	${Boost_LIBRARIES})
