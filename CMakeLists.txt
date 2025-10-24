cmake_minimum_required(VERSION 3.10)
project(chessboard)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 QUIET)

# Find ncurses
find_package(PkgConfig REQUIRED)
pkg_check_modules(NCURSES REQUIRED ncurses)

# Create executable
add_executable(chessboard 
    src/main.cpp
    src/chessboard_chars.cpp
    src/chessboard_sdl.cpp
    src/chess_pieces_sdl.cpp
    src/chessboard_ncurses.cpp
    src/chess_pieces_ncurses.cpp
)

# Link SDL2 if available
if(SDL2_FOUND)
    target_include_directories(chessboard PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(chessboard ${SDL2_LIBRARIES})
    target_compile_definitions(chessboard PRIVATE HAS_SDL2)
endif()

# Link ncurses
target_include_directories(chessboard PRIVATE ${NCURSES_INCLUDE_DIRS})
target_link_libraries(chessboard ${NCURSES_LIBRARIES})
