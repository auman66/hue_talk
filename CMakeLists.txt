cmake_minimum_required(VERSION 3.15)
project(hue_talk)

add_subdirectory(cpr)
add_subdirectory(json)
set(HUE_TALK_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "")
include_directories(${CPR_INCLUDE_DIRS})
include_directories(${JSON_INCLUDE_DIRS})
include_directories(${HUE_TALK_INCLUDE_DIRS})


set(CMAKE_CXX_STANDARD 14)

set(SOURCES main.cpp input_check.cpp HueLink.cpp ${HUE_TALK_INCLUDE_DIRS}/HueLink.h ${HUE_TALK_INCLUDE_DIRS}/input_check.h)
add_executable(hue_talk ${SOURCES})
target_link_libraries(hue_talk
        ${CPR_LIBRARIES}
        nlohmann_json::nlohmann_json)