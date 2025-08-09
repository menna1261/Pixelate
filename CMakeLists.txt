cmake_minimum_required(VERSION 3.10)
project(Pixelate)

set(CMAKE_CXX_STANDARD 17)

# Find GTKmm 3.0
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

# Include directories and flags
include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})
add_definitions(${GTKMM_CFLAGS_OTHER})

# Add your source files
add_executable(Pixelate main.cpp MainWindow.cpp)
include_directories(${GTKMM_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)
# Link with GTKmm
target_link_libraries(Pixelate ${GTKMM_LIBRARIES})
