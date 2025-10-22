cmake_minimum_required(VERSION 3.10)
project(SDLChessboard)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Try to find SDL2
find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(SDL2 sdl2)
endif()

# Create both executables
add_executable(chessboard_simple main_simple.cpp)

if(SDL2_FOUND)
    message(STATUS "Found SDL2: ${SDL2_LIBRARIES}")
    add_executable(sdl_chessboard main.cpp)
    target_link_libraries(sdl_chessboard ${SDL2_LIBRARIES})
    target_include_directories(sdl_chessboard PRIVATE ${SDL2_INCLUDE_DIRS})
else()
    message(WARNING "SDL2 not found. Building only ASCII version.")
endif()
