cmake_minimum_required(VERSION 3.12)

project(PathFindingStatVisualizer)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_compile_options(/utf-8)
endif()

file(GLOB ASSETS "./assets")
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR})

include_directories("thirdparty/include")

if (MSVC)
    link_directories("${CMAKE_SOURCE_DIR}/thirdparty/lib/msvc")
    set(SDL_LINK_LIBS SDL2 SDL2main SDL2_ttf SDL2_image SDL2_mixer)
elseif (MINGW)
    link_directories("${CMAKE_SOURCE_DIR}/thirdparty/lib/mingw")
    set(SDL_LINK_LIBS SDL2 SDL2_ttf SDL2_image SDL2_mixer)
endif()

add_subdirectory(imgui)
add_subdirectory(application)
add_subdirectory(algorithm)
add_subdirectory(Aframework)

set(PATH_FINDING_VISUALIZER_SOURCES
    "main.cpp"
    "status.h"
)

if(WIN32)
    enable_language(RC)
    configure_file(
        "${CMAKE_SOURCE_DIR}/assets/icon/app_icon.rc.in"
        "${CMAKE_BINARY_DIR}/app_icon.rc"
        @ONLY
    )
    list(APPEND PATH_FINDING_VISUALIZER_SOURCES "${CMAKE_BINARY_DIR}/app_icon.rc")
endif()

add_executable(PathFindingVisualizer ${PATH_FINDING_VISUALIZER_SOURCES})

if(WIN32)
    set_target_properties(PathFindingVisualizer PROPERTIES
        WIN32_EXECUTABLE "$<CONFIG:Release>"
    )
endif()

target_link_libraries(PathFindingVisualizer 
    applicationLib algorithmLib AframeworkLib imguiLib
    ${SDL_LINK_LIBS}
)
