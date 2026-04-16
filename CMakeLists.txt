#需求的最低cmake程序版本
cmake_minimum_required(VERSION 3.12)

#本工程的名字
project(A-StatVisualizer)

#本工程支持的C++版本
set(CMAKE_CXX_STANDARD 17)

#把需要拷贝的资源路径都放到ASSETS里
file(GLOB ASSETS "./assets")
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR})

include_directories("thirdparty/include")

link_directories("thirdparty/lib/x64")

add_subdirectory(imgui)
add_subdirectory(appliction)
add_subdirectory(algorithm)
add_subdirectory(Aframework)

#本工程所有cpp文件编译链接，生成exe
add_executable(PathFindingVisualizer "main.cpp"  "status.h")

target_link_libraries(PathFindingVisualizer 
    applicationLib algorithmLib AframeworkLib 
    SDL2.lib SDL2main.lib SDL2_ttf.lib SDL2_image.lib SDL2_mixer.lib imguiLib
)
