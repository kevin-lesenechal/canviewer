cmake_minimum_required(VERSION 3.0)

project(canviewer LANGUAGES CXX VERSION 0.1.0)
set(BIN_NAME "canviewer")

#---- OPTIONS -----------------------------------------------------------------#

set(GIT_EXECUTABLE "git" CACHE FILEPATH "git executable file or command")
set(CMAKE_AUTOMOC on)
set(CMAKE_AUTOUIC on)

#---- SOURCES -----------------------------------------------------------------#

set(SOURCES
    src/main.cpp
    src/application.hpp
    src/application.cpp
    src/frame.hpp
    src/socketcan_source.hpp
    src/socketcan_source.cpp
    src/ui/window.ui
    src/ui/window.hpp
    src/ui/window.cpp
    src/ui/frames_model.hpp
    src/ui/frames_model.cpp
    src/file_source.hpp
    src/file_source.cpp
    src/source_interface.hpp
    src/sample.hpp
    src/can_decoder.hpp
    src/can_decoder.cpp
)

set(TEST_SOURCES "")

#---- BUILD OPTIONS -----------------------------------------------------------#

include_directories(include ${CMAKE_BINARY_DIR}/include src)
set(CMAKE_CXX_STANDARD      17)
set(CMAKE_CXX_FLAGS         "-pedantic -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG   "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-s -O3 -flto")

#---- LIBRARIES ---------------------------------------------------------------#

find_package(Qt5Widgets)

set(LIBS Qt5::Widgets)

#------------------------------------------------------------------------------#

include(cmake/buildconfig.cmake)
include(cmake/tests.cmake)

#---- OUTPUT ------------------------------------------------------------------#

add_executable(${BIN_NAME} ${SOURCES})

target_link_libraries(${BIN_NAME} ${LIBS})
