cmake_minimum_required(VERSION 3.10)

project(fusion_bcf)

set(CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "/W4 /nologo /EHsc")

file(GLOB headers "./rapidxml/*.hpp")
add_executable(fusion_bcf main.cpp ${headers} FusionBCF.cpp FusionBCF.h )

