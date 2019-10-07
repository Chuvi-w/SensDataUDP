cmake_minimum_required(VERSION 3.8)


add_library(BoostQVM INTERFACE)
target_include_directories(BoostQVM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
