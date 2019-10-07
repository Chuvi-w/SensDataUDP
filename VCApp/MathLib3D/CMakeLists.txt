cmake_minimum_required(VERSION 3.8)

FILE(GLOB MathLib3D_src *.cpp *.c)
FILE(GLOB MathLib3D_h *.h)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(MathLib3D STATIC ${MathLib3D_src} ${MathLib3D_h})
target_include_directories(MathLib3D INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

FixStaticLinking()

SET_TARGET_PROPERTIES (MathLib3D PROPERTIES FOLDER Libraries)