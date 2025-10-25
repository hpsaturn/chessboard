cmake_minimum_required(VERSION 3.10)
project(ChessProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Check for SDL2
pkg_check_modules(SDL2 QUIET sdl2)
if(SDL2_FOUND)
    message(STATUS "SDL2 found: ${SDL2_VERSION}")
    add_definitions(-DHAVE_SDL2)
else()
    message(WARNING "SDL2 not found - SDL mode will be disabled")
endif()

# Check for SDL2_image
pkg_check_modules(SDL2_IMAGE QUIET SDL2_image)
if(SDL2_IMAGE_FOUND)
    message(STATUS "SDL2_image found: ${SDL2_IMAGE_VERSION}")
    add_definitions(-DHAVE_SDL2_IMAGE)
else()
    message(WARNING "SDL2_image not found - image loading will be disabled")
endif()

# Check for SDL2_ttf
pkg_check_modules(SDL2_TTF QUIET SDL2_ttf)
if(SDL2_TTF_FOUND)
    message(STATUS "SDL2_ttf found: ${SDL2_TTF_VERSION}")
    add_definitions(-DHAVE_SDL2_TTF)
else()
    message(WARNING "SDL2_ttf not found - text rendering will use fallback")
endif()

# Check for ncurses
find_package(Curses QUIET)
if(CURSES_FOUND)
    message(STATUS "ncurses found")
    add_definitions(-DHAVE_NCURSES)
else()
    message(WARNING "ncurses not found - ncurses mode will be disabled")
endif()

# Source files for main chess application
set(CHESS_SOURCES
    src/main.cpp
    src/chessboard_chars.cpp
    src/chessboard_chars_engine.cpp
    src/engine/gnuchess_adapter.cpp
    src/chess_pieces.h
    src/chess_pieces_ncurses.h
    src/chess_pieces_ncurses.cpp
    src/chess_pieces_sdl.h
    src/chess_pieces_sdl.cpp
)

# Add SDL sources if available
if(SDL2_FOUND)
    list(APPEND CHESS_SOURCES
        src/chessboard_sdl.cpp
    )
endif()

# Add ncurses sources if available
if(CURSES_FOUND)
    list(APPEND CHESS_SOURCES
        src/chessboard_ncurses.cpp
    )
endif()

# Add engine sources
list(APPEND CHESS_SOURCES
    src/engine/gnuchess_adapter.cpp
    src/engine/gnuchess_adapter.h
    src/engine/chess_engine.h
)

# Create main chess executable
add_executable(chess ${CHESS_SOURCES})

# Link libraries for main chess executable
if(SDL2_FOUND)
    target_link_libraries(chess ${SDL2_LIBRARIES})
    if(SDL2_IMAGE_FOUND)
        target_link_libraries(chess ${SDL2_IMAGE_LIBRARIES})
    endif()
    if(SDL2_TTF_FOUND)
        target_link_libraries(chess ${SDL2_TTF_LIBRARIES})
    endif()
endif()

if(CURSES_FOUND)
    target_link_libraries(chess ${CURSES_LIBRARIES})
endif()

# Create engine test executable
add_executable(engine_test src/engine_test.cpp src/engine/gnuchess_adapter.cpp)
target_include_directories(engine_test PRIVATE src)

# Install target
install(TARGETS chess DESTINATION bin)
