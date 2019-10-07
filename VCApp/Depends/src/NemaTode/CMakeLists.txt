cmake_minimum_required(VERSION 3.8)
set(NemaTode_HdrRoot "${CMAKE_CURRENT_SOURCE_DIR}/../../include")

FILE(GLOB NemaTode_src *.cpp)
FILE(GLOB NemaTode_h ${NemaTode_HdrRoot}/nmeaparse/*.h)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(NemaTode STATIC ${NemaTode_src} ${NemaTode_h})
target_include_directories(NemaTode PUBLIC ${NemaTode_HdrRoot})

FixStaticLinking()

SET_TARGET_PROPERTIES (NemaTode PROPERTIES FOLDER Libraries)