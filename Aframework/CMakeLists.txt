#递归本文件夹所有的.cpp文件
file(GLOB_RECURSE AFRAMEWORK ./ *.cpp)
add_library(AframeworkLib ${AFRAMEWORK})
