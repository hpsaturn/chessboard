cmake_minimum_required(VERSION 3.10)
project(ChessProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)

# Platform-specific dependency handling
if(WIN32)
    # Windows: Use vcpkg or find_package with fallback
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(yaml-cpp REQUIRED)
    
    # Include directories for Windows
    include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${YAML_CPP_INCLUDE_DIRS})
    
    # For ncurses on Windows, we'll skip it or use alternative
    message(STATUS "Skipping ncurses on Windows - using alternative or SDL only")
    
else()
    # Linux: Use pkg-config
    find_package(PkgConfig REQUIRED)
    
    # Check for SDL2
    pkg_check_modules(SDL2 QUIET sdl2)
    if(NOT SDL2_FOUND)
        message(FATAL_ERROR "SDL2 not found. Please install libsdl2-dev")
    endif()
    
    # Check for SDL2_image
    pkg_check_modules(SDL2_IMAGE QUIET SDL2_image)
    if(NOT SDL2_IMAGE_FOUND)
        message(FATAL_ERROR "SDL2_image not found. Please install libsdl2-image-dev")
    endif()
    
    # Check for SDL2_ttf
    pkg_check_modules(SDL2_TTF QUIET SDL2_ttf)
    if(NOT SDL2_TTF_FOUND)
        message(FATAL_ERROR "SDL2_ttf not found. Please install libsdl2-ttf-dev")
    endif()
    
    # Check for yaml-cpp
    pkg_check_modules(YAML_CPP QUIET yaml-cpp)
    if(NOT YAML_CPP_FOUND)
        message(FATAL_ERROR "yaml-cpp not found. Please install libyaml-cpp-dev")
    endif()
    
    # Check for ncurses
    pkg_check_modules(NCURSES QUIET ncurses)
    if(NOT NCURSES_FOUND)
        message(FATAL_ERROR "ncurses not found. Please install libncurses-dev")
    endif()
    
    # Include directories for Linux
    include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${YAML_CPP_INCLUDE_DIRS} ${NCURSES_INCLUDE_DIRS})
endif()

# Define resource paths
set(RESOURCE_PATH_DEVELOPMENT "res")
set(RESOURCE_PATH_INSTALLED "/usr/local/share/chess")

# Add executable
add_executable(chess
    src/main.cpp
    src/chessboard.cpp
    src/chessboard.h
    src/chessboard_ascii.cpp
    src/chessboard_ascii.h
    src/chessboard_sdl.cpp
    src/chessboard_sdl.h
    src/engine/engine.cpp
    src/engine/engine.h
    src/engine/move_generator.cpp
    src/engine/move_generator.h
    src/engine/board.cpp
    src/engine/board.h
    src/engine/evaluation.cpp
    src/engine/evaluation.h
    src/engine/search.cpp
    src/engine/search.h
    src/engine/transposition_table.cpp
    src/engine/transposition_table.h
    src/engine/uci.cpp
    src/engine/uci.h
)

# Link libraries
if(WIN32)
    target_link_libraries(chess
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${YAML_CPP_LIBRARIES}
        Threads::Threads
    )
else()
    target_link_libraries(chess
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${YAML_CPP_LIBRARIES}
        ${NCURSES_LIBRARIES}
        Threads::Threads
    )
endif()

# Add compile definitions for resource paths
target_compile_definitions(chess PRIVATE
    RESOURCE_PATH_DEVELOPMENT="${RESOURCE_PATH_DEVELOPMENT}"
    RESOURCE_PATH_INSTALLED="${RESOURCE_PATH_INSTALLED}"
)

# Copy resource files to build directory
file(GLOB_RECURSE RESOURCE_FILES "res/*")
foreach(RESOURCE_FILE ${RESOURCE_FILES})
    get_filename_component(RESOURCE_DIR "${RESOURCE_FILE}" DIRECTORY)
    get_filename_component(RESOURCE_NAME "${RESOURCE_FILE}" NAME)
    add_custom_command(
        TARGET chess POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${RESOURCE_FILE}"
                "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_DIR}/${RESOURCE_NAME}"
        COMMENT "Copying resource file: ${RESOURCE_NAME}"
    )
endforeach()

# Installation
install(TARGETS chess DESTINATION bin)
install(DIRECTORY res/ DESTINATION share/chess)
