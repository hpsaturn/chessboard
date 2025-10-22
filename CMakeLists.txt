cmake_minimum_required(VERSION 3.10)
project(Chessboard)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check for SDL2
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

# Add executable with SDL support
add_executable(chessboard main.cpp)

if(SDL2_FOUND)
    target_compile_definitions(chessboard PRIVATE HAS_SDL)
    target_include_directories(chessboard PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(chessboard ${SDL2_LIBRARIES})
    message(STATUS "SDL2 found - building with graphical support")
else()
    message(WARNING "SDL2 not found - building ASCII version only")
endif()

# Installation
install(TARGETS chessboard DESTINATION bin)
