project(sdk_launcher
		LANGUAGES CXX
		DESCRIPTION "Enemy Territory: Quake Wars SDK Front End"
		VERSION "1.5.12665.33243"
		HOMEPAGE_URL "http://www.enemyterritory.com")

include(CheckIPOSupported)
include(GNUInstallDirs)

set(PROJECT_NAME_PRETTY "SDK Launcher" CACHE STRING "" FORCE)
set(PROJECT_TARGET_NAME "SDKLauncher"  CACHE STRING "" FORCE)

if(PROJECT_IS_TOP_LEVEL)
	option(SDK_LAUNCHER_USE_LTO "Build SDK Launcher with link-time optimization enabled" OFF)

	# Set proper runpath
	set(CMAKE_SKIP_BUILD_RPATH OFF)
	set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
	set(CMAKE_INSTALL_RPATH $ORIGIN)

	# Compile with PIC
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)

	# Compile with LTO if supported
	set(SDK_LAUNCHER_USE_LTO_INTERNAL OFF)
	if(SDK_LAUNCHER_USE_LTO)
		check_ipo_supported(RESULT SDK_LAUNCHER_USE_LTO_INTERNAL)
	endif()
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ${SDK_LAUNCHER_USE_LTO_INTERNAL})

	# Set default install directory permissions
	set(CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

# Macro to set up many things at once for a given target
function(sdk_launcher_configure_target TARGET)
	# Include the precompile header
	target_precompile_headers(${TARGET} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/precompiled.h")

	# Define DEBUG macro
	target_compile_definitions(${TARGET} PRIVATE "$<$<CONFIG:Debug>:DEBUG>")

	# Set optimization flags
	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		# Build with debug friendly optimizations and debug symbols (MSVC defaults are fine)
		if(UNIX OR MINGW)
			target_compile_options(${TARGET} PRIVATE -Og -g)
		endif()
		if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT MSVC)
			target_compile_options(${TARGET} PRIVATE -fno-limit-debug-info)
		endif()
	else()
		# Build with optimizations and don't omit stack pointer for debugging (MSVC defaults are fine)
		if(UNIX OR MINGW)
			target_compile_options(${TARGET} PRIVATE -O2 -fno-omit-frame-pointer)
		endif()
	endif()

	if(WIN32 AND MSVC)
		get_target_property(TARGET_TYPE ${TARGET} TYPE)
		get_target_property(TARGET_IS_GUI ${TARGET} WIN32_EXECUTABLE)

		# Create PDBs in release
		if(MSVC AND ((TARGET_TYPE STREQUAL "SHARED_LIBRARY") OR (TARGET_TYPE STREQUAL "EXECUTABLE")))
			target_compile_options(
					${TARGET} PRIVATE
					"$<$<CONFIG:Release>:/Zi>")
			target_link_options(
					${TARGET} PRIVATE
					"$<$<CONFIG:Release>:/DEBUG>"
					"$<$<CONFIG:Release>:/OPT:REF>"
					"$<$<CONFIG:Release>:/OPT:ICF>")
		endif()

		# Don't show the console when running the executable
		if(MSVC AND TARGET_IS_GUI)
			target_link_options(
					${TARGET} PRIVATE
					"/ENTRY:mainCRTStartup")
		endif()
	endif()

	set_target_properties(
		${TARGET} PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY $<1:>${CMAKE_SOURCE_DIR}/sdk_output
		LIBRARY_OUTPUT_DIRECTORY $<1:>${CMAKE_SOURCE_DIR})
endfunction()

if(DEFINED QT_BASEDIR)
	string(REPLACE "\\" "/" QT_BASEDIR "${QT_BASEDIR}")

	# Add it to the prefix path so find_package can find it
	list(APPEND CMAKE_PREFIX_PATH "${QT_BASEDIR}")
	set(QT_INCLUDE "${QT_BASEDIR}/include")
	message(STATUS "Using ${QT_INCLUDE} as the Qt include directory")
endif()

# CMake has an odd policy that links a special link lib for Qt on newer versions of CMake
cmake_policy(SET CMP0020 NEW)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Generate config header
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in" "${CMAKE_BINARY_DIR}/config.h")
include_directories(${CMAKE_BINARY_DIR})

file(GLOB src_launcher_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
source_group("Source Files" FILES ${src_launcher_sources})

file(GLOB src_qt_resources "${CMAKE_CURRENT_SOURCE_DIR}/res/*.qrc" "${CMAKE_CURRENT_SOURCE_DIR}/res/config/*.json" "${CMAKE_CURRENT_SOURCE_DIR}/res/icons/*.png")
source_group("QT Resource Files" FILES ${src_qt_resources})

if(WIN32)
file(GLOB src_win32_resources "${CMAKE_CURRENT_SOURCE_DIR}/res/win32/*.rc" "${CMAKE_CURRENT_SOURCE_DIR}/res/win32/*.ico" "${CMAKE_CURRENT_SOURCE_DIR}/res/win32/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/res/win32/*.manifest")
source_group("Win32 Resource Files" FILES ${src_win32_resources})
endif()

# Create executable
add_executable(${PROJECT_TARGET_NAME} WIN32 ${src_launcher_sources} ${src_qt_resources} ${src_win32_resources})

sdk_launcher_configure_target(${PROJECT_TARGET_NAME})

target_link_libraries(
		${PROJECT_TARGET_NAME} PRIVATE
		Qt::Core
		Qt::Gui
		Qt::Widgets)

target_include_directories(
		${PROJECT_TARGET_NAME} PRIVATE
		"${QT_INCLUDE}"
		"${QT_INCLUDE}/QtCore"
		"${QT_INCLUDE}/QtGui"
		"${QT_INCLUDE}/QtWidgets")

# get absolute path to qmake, then use it to find windeployqt executable
get_target_property(_qmake_executable Qt::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

function(windeployqt target)
	# POST_BUILD step
	# - after build, we have a bin/lib for analyzing qt dependencies
	# - we run windeployqt on target and deploy Qt libs
	add_custom_command(TARGET ${target} POST_BUILD
		COMMAND "${_qt_bin_dir}/windeployqt.exe"
				--verbose 1
				$<$<CONFIG:Release>:--release>
				--no-svg
				--no-opengl
				--no-opengl-sw
				--no-compiler-runtime
				--no-system-d3d-compiler
				--translations en,es
				--skip-plugin-types imageformats,networkinformation,tls,generic,styles
				\"$<TARGET_FILE:${target}>\"
		COMMENT "Deploying Qt libraries using windeployqt for compilation target '${target}' ..."
	)
endfunction()

windeployqt(${PROJECT_TARGET_NAME})