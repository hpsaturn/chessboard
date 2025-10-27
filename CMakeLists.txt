cmake_minimum_required(VERSION 3.10)
project(ChessProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# Check for ncurses
find_package(Curses QUIET)
if(CURSES_FOUND)
    message(STATUS "ncurses found")
    add_definitions(-DHAVE_NCURSES)
    include_directories(${CURSES_INCLUDE_DIRS})
else()
    message(WARNING "ncurses not found - ncurses mode will be disabled")
endif()

# Source files for main chess application
set(CHESS_SOURCES
    src/main.cpp
    src/chessboard_chars.cpp
    src/chessboard_chars_engine.cpp
    src/chessboard_chars_engine.h
    src/chess_pieces.h
    src/chess_pieces_ncurses.h
    src/chess_pieces_ncurses.cpp
    src/chess_pieces_sdl.h
    src/chess_pieces_sdl.cpp
    src/engine/uci_engine.cpp
    src/engine/uci_engine.h
)

# Add SDL sources if available
if(SDL2_FOUND)
    list(APPEND CHESS_SOURCES
        src/chessboard_sdl_fixed.cpp src/chess_game_logic.cpp
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

# Create engine test executables (only if files exist)
if(EXISTS "${CMAKE_SOURCE_DIR}/src/engine_test.cpp")
    add_executable(engine_test src/engine_test.cpp)
    target_include_directories(engine_test PRIVATE src)
endif()

# Install target
install(TARGETS chess DESTINATION bin)
