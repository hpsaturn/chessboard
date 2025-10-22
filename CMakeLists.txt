cmake_minimum_required(VERSION 3.10)
project(Chessboard)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check for SDL2
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 sdl2)

# Add definitions based on SDL2 availability
if(SDL2_FOUND)
    add_definitions(-DHAS_SDL2)
    message(STATUS "SDL2 found: ${SDL2_VERSION}")
else()
    message(WARNING "SDL2 not found - SDL2 mode will not be available")
endif()

# Create executable
add_executable(chessboard
    src/main.cpp
    src/chessboard_chars.cpp
    src/chessboard_sdl.cpp
)

# Link SDL2 if available
if(SDL2_FOUND)
    target_link_libraries(chessboard ${SDL2_LIBRARIES})
    target_include_directories(chessboard PRIVATE ${SDL2_INCLUDE_DIRS})
endif()

# Set output directory
set_target_properties(chessboard PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
