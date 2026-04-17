#递归本文件夹所有的.cpp文件
file(GLOB_RECURSE AFRAMEWORK CONFIGURE_DEPENDS ./*.cpp)
add_library(AframeworkLib ${AFRAMEWORK})
