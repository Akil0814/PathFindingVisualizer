cmake_minimum_required(VERSION 3.12)

project(PathFindingStatVisualizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

file(GLOB ASSETS "./assets")
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR})

set(SDL_LINK_LIBS)

if(WIN32)
    include_directories("thirdparty/include")

    if(MSVC)
        link_directories("${CMAKE_SOURCE_DIR}/thirdparty/lib/msvc")
        set(SDL_LINK_LIBS SDL2 SDL2main SDL2_ttf SDL2_image SDL2_mixer)
    elseif(MINGW)
        link_directories("${CMAKE_SOURCE_DIR}/thirdparty/lib/mingw")
        add_compile_definitions(SDL_MAIN_HANDLED)
        set(SDL_LINK_LIBS SDL2 SDL2_ttf SDL2_image SDL2_mixer)
    else()
        message(FATAL_ERROR "Unsupported Windows compiler. Use MSVC or MinGW, or add SDL library paths for this compiler.")
    endif()
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED IMPORTED_TARGET SDL2_image)
    pkg_check_modules(SDL2_TTF REQUIRED IMPORTED_TARGET SDL2_ttf)
    pkg_check_modules(SDL2_MIXER REQUIRED IMPORTED_TARGET SDL2_mixer)

    include_directories(
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
    )
    add_compile_options(
        ${SDL2_CFLAGS_OTHER}
        ${SDL2_IMAGE_CFLAGS_OTHER}
        ${SDL2_TTF_CFLAGS_OTHER}
        ${SDL2_MIXER_CFLAGS_OTHER}
    )
    set(SDL_LINK_LIBS
        PkgConfig::SDL2
        PkgConfig::SDL2_IMAGE
        PkgConfig::SDL2_TTF
        PkgConfig::SDL2_MIXER
    )
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

add_custom_command(TARGET PathFindingVisualizer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:PathFindingVisualizer>/assets"
)
