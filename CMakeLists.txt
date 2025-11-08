cmake_minimum_required(VERSION 3.16)
project(chess VERSION 1.0.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Threads package (required for both platforms)
find_package(Threads REQUIRED)

# Define resource paths for development and installation
add_compile_definitions(
    RESOURCE_PATH_DEVELOPMENT="res"
    RESOURCE_PATH_INSTALLED="/usr/local/share/chess"
)

# Copy resource files to build directory
file(COPY res/ DESTINATION ${CMAKE_BINARY_DIR}/res)

# Find dependencies based on platform
if(WIN32)
    # Windows: Try vcpkg first, then fallback to manual paths
    message(STATUS "Configuring for Windows build")
    
    # Try to find packages through vcpkg (note: vcpkg uses lowercase)
    find_package(sdl2 QUIET)
    find_package(sdl2-image QUIET)
    find_package(sdl2-ttf QUIET)
    find_package(yaml-cpp QUIET)
    
    # If packages not found, provide helpful error message
    if(NOT sdl2_FOUND)
        message(FATAL_ERROR "SDL2 not found on Windows. Please ensure vcpkg is properly set up with: vcpkg install sdl2 sdl2-image sdl2-ttf yaml-cpp")
    endif()
    
    # Include directories for Windows
    include_directories(${sdl2_INCLUDE_DIRS} ${sdl2-image_INCLUDE_DIRS} ${sdl2-ttf_INCLUDE_DIRS} ${yaml-cpp_INCLUDE_DIRS})
    
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
file(GLOB SOURCES 
    "src/*.cpp"
    "src/engine/*.cpp"
)

add_executable(chess ${SOURCES})

# Link libraries
if(WIN32)
    target_link_libraries(chess 
        ${sdl2_LIBRARIES} 
        ${sdl2-image_LIBRARIES} 
        ${sdl2-ttf_LIBRARIES} 
        ${yaml-cpp_LIBRARIES}
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
