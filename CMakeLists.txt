cmake_minimum_required(VERSION 3.10)
project(ChessProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Git version extraction at build time
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

# Build info (full git description)
set(BUILD_INFO "${GIT_VERSION}")

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# Check for SDL2
pkg_check_modules(SDL2 QUIET sdl2)
if(SDL2_FOUND)
    message(STATUS "SDL2 found: ${SDL2_VERSION}")
    include_directories(${SDL2_INCLUDE_DIRS})
else()
    message(ERROR "SDL2 not found! Please install libsdl2-dev")
endif()

# Check for SDL2_image
pkg_check_modules(SDL2_IMAGE QUIET SDL2_image)
if(SDL2_IMAGE_FOUND)
    message(STATUS "SDL2_image found: ${SDL2_IMAGE_VERSION}")
    include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
else()
    message(ERROR "SDL2_image not found Please install libsdl2-image-dev")
endif()

# Check for SDL2_ttf
pkg_check_modules(SDL2_TTF QUIET SDL2_ttf)
if(SDL2_TTF_FOUND)
    message(STATUS "SDL2_ttf found: ${SDL2_TTF_VERSION}")
    include_directories(${SDL2_TTF_INCLUDE_DIRS})
else()
    message(ERROR "SDL2_ttf not found - Please install libsdl2-ttf-dev")
endif()

# Check for yaml-cpp
pkg_check_modules(YAML_CPP QUIET yaml-cpp)
if(YAML_CPP_FOUND)
    message(STATUS "yaml-cpp found: ${YAML_CPP_VERSION}")
    include_directories(${YAML_CPP_INCLUDE_DIRS})
else()
    message(ERROR "yaml-cpp not found! Please install libyaml-cpp-dev")
endif()

# Check for ncurses
find_package(Curses QUIET)
if(CURSES_FOUND)
    message(STATUS "ncurses found")
    add_definitions(-DHAVE_NCURSES)
    include_directories(${CURSES_INCLUDE_DIRS})
else()
    message(WARNING "ncurses not found - ncurses mode will be disabled")
endif()

# Define resource paths
# Development path: relative to executable in build directory
# Installation path: /usr/local/share/chess (or custom prefix)
set(RESOURCE_PATH_DEVELOPMENT "res")
set(RESOURCE_PATH_INSTALLED "${CMAKE_INSTALL_PREFIX}/share/chess")

# Source files for main chess application
set(CHESS_SOURCES
    src/main.cpp
    src/chessboard_chars.cpp
    src/chess_pieces.h
    src/chess_pieces_ncurses.h
    src/chess_pieces_ncurses.cpp
    src/chess_pieces_sdl.h
    src/chess_pieces_sdl.cpp
    src/engine/uci_engine.cpp
    src/engine/uci_engine.h
    src/engine/bitboard.cpp
    src/chess_game_logic.cpp
    src/config_manager.cpp
    src/config_manager.h
    src/game_state_manager.cpp
    src/game_state_manager.h
)

# Add SDL sources if available
if(SDL2_FOUND)
    list(APPEND CHESS_SOURCES
        src/chessboard_sdl.cpp 
src/settings_modal.cpp
    src/settings_modal.h
        src/gameinfo_modal.h
        src/gameinfo_modal.cpp
        src/about_modal.h
        src/about_modal.cpp
    src/version.cpp
        src/gamestates_modal.h
        src/gamestates_modal.cpp
        src/help_modal.h
        src/help_modal.cpp
    )
endif()

# Add ncurses sources if available
if(CURSES_FOUND)
    list(APPEND CHESS_SOURCES
        src/chessboard_ncurses.cpp
    )
endif()

# Create main chess executable
add_executable(chess ${CHESS_SOURCES})

# Define version information at compile time
target_compile_definitions(chess PRIVATE 
    PROJECT_VERSION="${PROJECT_VERSION}"
    GIT_VERSION="${GIT_VERSION}"
    BUILD_INFO="${BUILD_INFO}"
    RESOURCE_PATH_DEVELOPMENT="${RESOURCE_PATH_DEVELOPMENT}"
    RESOURCE_PATH_INSTALLED="${RESOURCE_PATH_INSTALLED}"
)

# Link libraries for main chess executable
if(SDL2_FOUND)
    target_link_libraries(chess PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${SDL2_LIBRARIES}) 
    if(SDL2_IMAGE_FOUND)
        target_link_libraries(chess PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${SDL2_IMAGE_LIBRARIES})
    endif()
    if(SDL2_TTF_FOUND)
        target_link_libraries(chess PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${SDL2_TTF_LIBRARIES})
    endif()
endif()

if(CURSES_FOUND)
    target_link_libraries(chess PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${CURSES_LIBRARIES})
endif()

if(YAML_CPP_FOUND)
    target_link_libraries(chess PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${YAML_CPP_LIBRARIES})
endif()

# Create engine test executables (only if files exist)
if(EXISTS "${CMAKE_SOURCE_DIR}/src/engine_test.cpp")
    add_executable(engine_test src/engine_test.cpp)
    target_include_directories(engine_test PRIVATE src)
endif()

# Copy resources directory to build directory
add_custom_command(TARGET chess POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/res
            $<TARGET_FILE_DIR:chess>/res
    COMMENT "Copying resources to build directory"
)

# Install targets
install(TARGETS chess DESTINATION bin)

# Install resources directory
install(DIRECTORY res/
        DESTINATION share/chess
        PATTERN ".gitkeep" EXCLUDE
)
