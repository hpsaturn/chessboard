cmake_minimum_required(VERSION 3.10)
project(SDLChessboard)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Try to find SDL2 using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

# Create executable
add_executable(sdl_chessboard main.cpp)

# Link SDL2
target_link_libraries(sdl_chessboard ${SDL2_LIBRARIES})

# Include directories
target_include_directories(sdl_chessboard PRIVATE ${SDL2_INCLUDE_DIRS})
