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
	fonttext
	image
	image3d
	input
	parse
	renderer
	shader 
	systems
	texture
	time
	window
	viewport)

INCLUDE_DIRECTORIES(${SGE_INCLUDE_DIRS})
ADD_DEFINITIONS(${SGE_CFLAGS})
# sge end

# TODO
SET_SOURCE_FILES_PROPERTIES(
	media_path.cpp
	PROPERTIES
	COMPILE_FLAGS
	-DMEDIA_PATH="\\\"${media_path}\\\""
)

SET_SOURCE_FILES_PROPERTIES(
	src/media_path.cpp
	PROPERTIES
	COMPILE_FLAGS
	-DMEDIA_PATH="\\\"${CMAKE_INSTALL_PREFIX}/share/games/${PROJECT_NAME}/media\\\""
)

ADD_EXECUTABLE(
	app
	src/config_wrapper.cpp
	src/create_cube.cpp
	src/create_shader.cpp
	src/json/is_null.cpp
	src/json/merge_trees.cpp
	src/json/parse_rgba8_color.cpp
	src/json/process_option.cpp
	src/json/string_to_value.cpp
	src/name.cpp
	src/main.cpp
  src/media_path.cpp
	src/shadow_volume.cpp
  src/texture3d.cpp
)

TARGET_LINK_LIBRARIES(
	app
	${Boost_LIBRARIES}
	${Fcppt_LIBRARIES}
	${SGE_LIBRARIES}
)
