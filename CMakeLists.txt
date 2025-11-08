cmake_minimum_required(VERSION 3.10)
project(ChessProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)

# Platform-specific dependency handling
if(WIN32)
    # Windows: Use vcpkg or find_package
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
    else()
        message(WARNING "ncurses not found - some features may be limited")
    endif()
endif()

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Create executable
add_executable(chess ${SOURCES})

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
        Threads::Threads
    )
    
    if(CURSES_FOUND)
        target_link_libraries(chess ${CURSES_LIBRARIES})
    endif()
endif()

# Install target
install(TARGETS chess DESTINATION bin)
