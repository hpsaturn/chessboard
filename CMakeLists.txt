cmake_minimum_required(VERSION 3.10)
project(ChessProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Normal git repository - extract version from git
execute_process(
    COMMAND git describe --tags --always --dirty
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE GIT_RESULT
)

if(GIT_RESULT EQUAL 0)
    message(STATUS "Git version: ${GIT_VERSION}")
    # Extract just the tag version (remove commit hash and dirty flag)
    execute_process(
        COMMAND git describe --tags --abbrev=0
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE PROJECT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE TAG_RESULT
    )
    message(STATUS "Project version: ${PROJECT_VERSION}")
else()
    # Check if we're in a Buildroot environment
    if(DEFINED ENV{BR2_VERSION})
        message(STATUS "Buildroot environment detected")
        set(GIT_VERSION "v0.1.4-buildroot")
        message(STATUS "Using Buildroot version: ${GIT_VERSION}")
    else()
        set(GIT_VERSION "v0.1.4-unknown")
        message(WARNING "Could not determine git version, using default: ${GIT_VERSION}")
    endif()
endif()

# Build info (full git description)
set(BUILD_INFO "${GIT_VERSION}")

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

pkg_check_modules(SDL2 QUIET sdl2)
if(SDL2_FOUND)
    message(STATUS "SDL2 found: ${SDL2_VERSION}")
else()
    message(FATAL_ERROR "SDL2 not found. Please install SDL2 development libraries")
endif()

pkg_check_modules(SDL2_IMAGE QUIET SDL2_image)
if(SDL2_IMAGE_FOUND)
    message(STATUS "SDL2_image found: ${SDL2_IMAGE_VERSION}")
else()
    message(FATAL_ERROR "SDL2_image not found. Please install SDL2_image development libraries")
endif()

pkg_check_modules(SDL2_TTF QUIET SDL2_ttf)
if(SDL2_TTF_FOUND)
    message(STATUS "SDL2_ttf found: ${SDL2_TTF_VERSION}")
else()
    message(FATAL_ERROR "SDL2_ttf not found. Please install SDL2_ttf development libraries")
endif()

pkg_check_modules(YAML_CPP QUIET yaml-cpp)
if(YAML_CPP_FOUND)
    message(STATUS "yaml-cpp found: ${YAML_CPP_VERSION}")
else()
    message(FATAL_ERROR "yaml-cpp not found. Please install yaml-cpp development libraries")
endif()

pkg_check_modules(NCURSES QUIET ncurses)
if(NCURSES_FOUND)
    message(STATUS "ncurses found: ${NCURSES_VERSION}")
else()
    message(FATAL_ERROR "ncurses not found. Please install ncurses development libraries")
endif()

# Include directories
include_directories(src)

# Define resource paths with Buildroot support
# Development path: relative to executable in build directory
# Installation path: /usr/share/chess for Buildroot, /usr/local/share/chess for normal install
set(RESOURCE_PATH_DEVELOPMENT "res")

# Detect Buildroot environment and adjust installation path
# Check multiple indicators of Buildroot environment
if(DEFINED ENV{BR2_VERSION} OR 
   DEFINED ENV{BUILDROOT_BUILD} OR
   DEFINED ENV{BUILDROOT_CONFIG} OR
   "${CMAKE_SYSTEM_NAME}" STREQUAL "Buildroot")
    set(RESOURCE_PATH_INSTALLED "/usr/share/chess")
    message(STATUS "Buildroot detected: using /usr/share/chess for resources")
    set(RESOURCE_PATH_INSTALLED "/usr/share/chess")
else()
    set(RESOURCE_PATH_INSTALLED "${CMAKE_INSTALL_PREFIX}/share/chess")
    message(STATUS "Normal install: using ${RESOURCE_PATH_INSTALLED} for resources")
endif()

file(GLOB SOURCES 
    "src/*.cpp"
    "src/engine/*.cpp"
)

add_executable(chess ${SOURCES})

target_compile_definitions(chess PRIVATE
    PROJECT_VERSION="${PROJECT_VERSION}"
    GIT_VERSION="${GIT_VERSION}"
    BUILD_INFO="${BUILD_INFO}"
    RESOURCE_PATH_DEVELOPMENT="${RESOURCE_PATH_DEVELOPMENT}"
    RESOURCE_PATH_INSTALLED="${RESOURCE_PATH_INSTALLED}"
)

target_link_libraries(chess
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${YAML_CPP_LIBRARIES}
    ${NCURSES_LIBRARIES}
    Threads::Threads
)

# Copy resource files to build directory
add_custom_command(TARGET chess POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res
        $<TARGET_FILE_DIR:chess>/res
    COMMENT "Copying resource files to build directory"
)

install(TARGETS chess DESTINATION bin)
install(DIRECTORY res/ DESTINATION share/chess)

