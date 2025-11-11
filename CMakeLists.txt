cmake_minimum_required(VERSION 3.10)
project(ChessProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Version detection priority:
# 1. Buildroot environment variables (BR2_VERSION, CHESSBOARD_VERSION)
# 2. Git tags and description
# 3. Fallback to default version

# Check if we're in a Buildroot environment
if(DEFINED ENV{BR2_VERSION})
    message(STATUS "Buildroot environment detected")
    
    # Use Buildroot's version if available
    if(DEFINED ENV{CHESSBOARD_VERSION})
        set(GIT_VERSION "$ENV{CHESSBOARD_VERSION}")
        set(PROJECT_VERSION "v0.1.3")  # Use latest known tag for Buildroot
        message(STATUS "Using Buildroot version: ${GIT_VERSION}")
    else()
        # Fallback for Buildroot without specific version
        set(GIT_VERSION "v0.1.3-buildroot")
        set(PROJECT_VERSION "v0.1.3")
        message(STATUS "Using default Buildroot version")
    endif()
else()
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
    else()
        set(GIT_VERSION "v0.1.3-unknown")
        message(WARNING "Could not determine git version, using default: ${GIT_VERSION}")
    endif()

    # Extract just the tag version (remove commit hash and dirty flag)
    execute_process(
        COMMAND git describe --tags --abbrev=0
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE PROJECT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE TAG_RESULT
    )

    if(TAG_RESULT EQUAL 0)
        message(STATUS "Project version: ${PROJECT_VERSION}")
    else()
        set(PROJECT_VERSION "v0.1.3")
        message(WARNING "Could not determine project version, using default: ${PROJECT_VERSION}")
    endif()
endif()

# Build info (full git description)
set(BUILD_INFO "${GIT_VERSION}")

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# Check for SDL2
pkg_check_modules(SDL2 QUIET sdl2)
if(SDL2_FOUND)
    message(STATUS "SDL2 found: ${SDL2_VERSION}")
else()
    message(FATAL_ERROR "SDL2 not found. Please install SDL2 development libraries")
endif()

# Check for SDL2_image
pkg_check_modules(SDL2_IMAGE QUIET SDL2_image)
if(SDL2_IMAGE_FOUND)
    message(STATUS "SDL2_image found: ${SDL2_IMAGE_VERSION}")
else()
    message(FATAL_ERROR "SDL2_image not found. Please install SDL2_image development libraries")
endif()

# Check for SDL2_ttf
pkg_check_modules(SDL2_TTF QUIET SDL2_ttf)
if(SDL2_TTF_FOUND)
    message(STATUS "SDL2_ttf found: ${SDL2_TTF_VERSION}")
else()
    message(FATAL_ERROR "SDL2_ttf not found. Please install SDL2_ttf development libraries")
endif()

# Check for yaml-cpp
pkg_check_modules(YAML_CPP QUIET yaml-cpp)
if(YAML_CPP_FOUND)
    message(STATUS "yaml-cpp found: ${YAML_CPP_VERSION}")
else()
    message(FATAL_ERROR "yaml-cpp not found. Please install yaml-cpp development libraries")
endif()

# Check for ncurses
find_package(PkgConfig REQUIRED)
pkg_check_modules(NCURSES QUIET ncurses)
if(NCURSES_FOUND)
    message(STATUS "ncurses found: ${NCURSES_VERSION}")
else()
    message(FATAL_ERROR "ncurses not found. Please install ncurses development libraries")
endif()

# Include directories
include_directories(src)

# Define resource paths
# Development path: relative to executable in build directory
# Installation path: /usr/local/share/chess (or custom prefix)
set(RESOURCE_PATH_DEVELOPMENT "res")
set(RESOURCE_PATH_INSTALLED "${CMAKE_INSTALL_PREFIX}/share/chess")                                                                                                             

file(GLOB SOURCES 
    "src/*.cpp"
    "src/engine/*.cpp"
)

# Add executable
add_executable(chess ${SOURCES})

# Set compile definitions for version info and resource paths
target_compile_definitions(chess PRIVATE
    PROJECT_VERSION="${PROJECT_VERSION}"
    GIT_VERSION="${GIT_VERSION}"
    BUILD_INFO="${BUILD_INFO}"
    RESOURCE_PATH_DEVELOPMENT="${RESOURCE_PATH_DEVELOPMENT}"
    RESOURCE_PATH_INSTALLED="${RESOURCE_PATH_INSTALLED}"
)

# Link libraries
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
        ${CMAKE_BINARY_DIR}/res
    COMMENT "Copying resource files to build directory"
)

# Installation
install(TARGETS chess DESTINATION bin)
install(DIRECTORY res/ DESTINATION share/chess)
